/* ==============================================================================
 
 This file is part of the POLYTEMPO software package.
 Copyright (c) 2016 - Zurich University of the Arts,
 ICST Institute for Computer Music and Sound Technology
 http://www.icst.net
 
 Author: Philippe Kocher
 
 POLYTEMPO is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 POLYTEMPO is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this software. If not, see <http://www.gnu.org/licenses/>.
 
 ============================================================================== */

#include "Polytempo_Composition.h"
#include "../Application/PolytempoComposer/Polytempo_ComposerApplication.h"
#include "../Scheduler/Polytempo_ScoreScheduler.h"
#include "../Preferences/Polytempo_StoredPreferences.h"
#include "../Views/PolytempoComposer/Polytempo_DialogWindows.h"
#include "../Audio+Midi/Polytempo_AudioClick.h"


Polytempo_Composition::Polytempo_Composition()
{
    score = new Polytempo_Score();
    Polytempo_ScoreScheduler::getInstance()->setScore(score);
}

Polytempo_Composition::~Polytempo_Composition()
{
    score = nullptr;
    clearSingletonInstance();
}

juce_ImplementSingleton (Polytempo_Composition)

void Polytempo_Composition::setMainWindow(DocumentWindow *w)
{
    mainWindow = w;
}

int sequenceCounter = 0;

void Polytempo_Composition::updateContent()
{
    findCoincidingControlPoints();
    
    if(mainWindow) mainWindow->repaint();
    
    Polytempo_ComposerApplication::getCommandManager().commandStatusChanged(); // update menubar
    Polytempo_AudioClick::getInstance()->setNumVoices(sequenceCounter);
    
    scoreNeedsUpdate = true;

    // the score mustn't be updated while it is being read by the scheduler
    if(!Polytempo_ScoreScheduler::getInstance()->isRunning()) updateScore();
}

void Polytempo_Composition::setDirty(bool flag) { dirty = flag; }
bool Polytempo_Composition::isDirty() { return dirty; }

void Polytempo_Composition::addSequence()
{
    Polytempo_Sequence *sequence;
    
    sequence = new Polytempo_Sequence();
    
    sequence->setName("Sequence "+String(++sequenceCounter));
    
    switch(sequences.size() % 3)
    {
        case 0:
            sequence->setColour(Colours::royalblue);
            break;
        case 1:
            sequence->setColour(Colours::orangered);
            break;
        case 2:
            sequence->setColour(Colours::forestgreen);
    }
    
    sequences.add(sequence);

    selectedSequenceIndex = sequences.indexOf(sequence);
    sequence->setIndex(sequences.indexOf(sequence));
    
    updateContent();
    dirty = true;
}

void Polytempo_Composition::removeSequence(int index)
{
    sequences.remove(index);
    
    if(selectedSequenceIndex > sequences.size() - 1) selectedSequenceIndex--;

    updateContent();
    dirty = true;
}

int  Polytempo_Composition::getNumberOfSequences()
{
    return sequences.size();
}


Polytempo_Sequence* Polytempo_Composition::getSequence(int index)
{
    if(index < 0 || index >= sequences.size())
        return nullptr;
    else
        return sequences[index];
}

Polytempo_Sequence* Polytempo_Composition::getSelectedSequence()
{
    return getSequence(selectedSequenceIndex);
}

void Polytempo_Composition::setSelectedSequenceIndex(int i)
{
    selectedSequenceIndex = i;
    selectedControlPointIndex = -1;
}

int Polytempo_Composition::getSelectedSequenceIndex()
{
    return selectedSequenceIndex;
}

void Polytempo_Composition::setSelectedControlPointIndex(int i)
{
    selectedControlPointIndex = i;
}

int Polytempo_Composition::getSelectedControlPointIndex()
{
    return selectedControlPointIndex;
}

bool Polytempo_Composition::isSelectedControlPointRemovable()
{
    if(selectedControlPointIndex < 1 ||
       getSelectedSequence()->getControlPoints()->size() == 2)
        return false;
    else
        return true;
}

void Polytempo_Composition::updateScore()
{
    if(!scoreNeedsUpdate) return;
    
    //DBG("update score");
    Polytempo_Sequence *sequence;
    Array <Polytempo_Event*> events;
    int i = -1;
    
    score->clear(true);
    
    while((sequence = getSequence(++i)) != nullptr)
    {
        sequence->updateEvents();
        score->addEvents(sequence->getEvents());
    }
    
    scoreNeedsUpdate = false;
}

void Polytempo_Composition::findCoincidingControlPoints()
{
    Polytempo_Sequence *sequence0, *sequence1;
    Polytempo_ControlPoint *controlPoint0, *controlPoint1;
    
    int i = -1;
    while((sequence0 = getSequence(++i)) != nullptr)
    {
        int j = -1;
        while((controlPoint0 = sequence0->getControlPoint(++j)) != nullptr)
        {
            controlPoint0->isCoinciding = false;

            int ii = -1;
            while((sequence1 = getSequence(++ii)) != nullptr)
            {
                if(sequence0 == sequence1) continue;
                
                int jj = -1;
                while((controlPoint1 = sequence1->getControlPoint(++jj)) != nullptr)
                {
                    if(controlPoint0->time == controlPoint1->time)  controlPoint0->isCoinciding = true;
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
#pragma mark -
#pragma mark file i/o

static void unsavedChangesCallback(int modalResult, Polytempo_YesNoCancelAlert::callbackTag tag)
{
    if(modalResult == 0)      return;                                                   // cancel
    else if(modalResult == 1) Polytempo_Composition::getInstance()->saveToFile();       // yes
    else if(modalResult == 2) Polytempo_Composition::getInstance()->setDirty(false);    // no
    
    if(tag == Polytempo_YesNoCancelAlert::applicationQuitTag) dynamic_cast<Polytempo_ComposerApplication*>(JUCEApplication::getInstance())->applicationShouldQuit();
    else if(tag == Polytempo_YesNoCancelAlert::openDocumentTag) Polytempo_Composition::getInstance()->openFile();
    else if(tag == Polytempo_YesNoCancelAlert::newDocumentTag) Polytempo_Composition::getInstance()->newComposition();
}

void Polytempo_Composition::unsavedChangesAlert(Polytempo_YesNoCancelAlert::callbackTag tag)
{
    String title;
    Polytempo_YesNoCancelAlert::show(title << "Do you want to save the changes to \"" << compositionFile.getFileNameWithoutExtension().toRawUTF8() << "\"?", "If you don't save your changes will be lost.", ModalCallbackFunction::create(unsavedChangesCallback, tag));
}

void Polytempo_Composition::newComposition()
{
    if(dirty)
    {
        unsavedChangesAlert(Polytempo_YesNoCancelAlert::newDocumentTag);
        return;
    }

    sequences.clear();
    Polytempo_Composition::getInstance()->addSequence(); // one sequence to start with
    setDirty(false);
    
    compositionFile = File();
    mainWindow->setName("Untitled");
}

void Polytempo_Composition::openFile()
{
    if(dirty)
    {
        unsavedChangesAlert(Polytempo_YesNoCancelAlert::openDocumentTag);
        return;
    }
       
    File directory(Polytempo_StoredPreferences::getInstance()->getProps().getValue("compositionFileDirectory"));
    FileChooser fileChooser("Open Score File", directory, "*.json;*.ptcom", true);
    
    if(fileChooser.browseForFileToOpen())
    {
        openFile(fileChooser.getResult());
    }
}

void Polytempo_Composition::openFile(File file)
{
    if(!file.existsAsFile()) return;
    
    if(readJSONfromFile(file))
    {
        compositionFile = file;
        dirty = false;
        
        mainWindow->setName(compositionFile.getFileNameWithoutExtension());
        Polytempo_StoredPreferences::getInstance()->getProps().setValue("compositionFileDirectory", compositionFile.getParentDirectory().getFullPathName());
        Polytempo_StoredPreferences::getInstance()->recentFiles.addFile(compositionFile);
    }
}


void Polytempo_Composition::saveToFile()
{
    if(!compositionFile.existsAsFile())
    {
        File directory(Polytempo_StoredPreferences::getInstance()->getProps().getValue("compositionFileDirectory"));
        FileChooser fileChooser("Save Composition", directory, "*.ptcom", true);

        if(fileChooser.browseForFileToSave(true))
        {
            File file = fileChooser.getResult();
            File tempFile(file.getParentDirectory().getFullPathName()+String("/~temp.ptcom"));
            writeJSONtoFile(tempFile);
            tempFile.copyFileTo(file);
            tempFile.deleteFile();
            dirty = false;
            compositionFile = file;
            mainWindow->setName(compositionFile.getFileNameWithoutExtension());
            Polytempo_StoredPreferences::getInstance()->getProps().setValue("compositionFileDirectory", compositionFile.getParentDirectory().getFullPathName());
        }
    }
    else
    {
        File tempFile(compositionFile.getParentDirectory().getFullPathName()+String("/~temp.ptcom"));
        writeJSONtoFile(tempFile);
        tempFile.copyFileTo(compositionFile);
        tempFile.deleteFile();
        dirty = false;
    }
}

void Polytempo_Composition::writeJSONtoFile(File file)
{
    DynamicObject* scoreObject = new DynamicObject();
    var json(scoreObject); // store the object in a var
    
    int i = 0;
    for(Polytempo_Sequence *seq : sequences)
    {
        scoreObject->setProperty(String(i++),seq->getObject());
    }
    
    String jsonString = JSON::toString(json,false);

    FileOutputStream stream(file);
    stream.writeString(jsonString);
}

bool Polytempo_Composition::readJSONfromFile(File file)
{
    var jsonVar = JSON::parse(file);
    
    if(jsonVar == var())
    {
        Polytempo_Alert::show("Error", "Not a valid JSON file:\n" + file.getFileName());
        return false;
    }

    NamedValueSet jsonSequences = jsonVar.getDynamicObject()->getProperties();
    if(jsonSequences.size() < 1)
    {
        Polytempo_Alert::show("Error", "No Sequences found in file:\n" + file.getFileName());
        return false;
    }

    sequences.clear();

    // iterate and addSequence
    DynamicObject* jsonObject;
    for(int i=0; i < jsonSequences.size(); i++)
    {
        Polytempo_Sequence *sequence = new Polytempo_Sequence();
        sequence->setObject(jsonObject = jsonSequences.getValueAt(i).getDynamicObject());
        sequence->setIndex(i);
        sequences.add(sequence);
    }
    
    selectedSequenceIndex = 0;
    
    updateContent();
    return true;
}


//------------------------------------------------------------------------------
#pragma mark -
#pragma mark export

void Polytempo_Composition::exportSelectedSequence()
{
    if(score == nullptr) return;
 
    exportAll = false;
    Polytempo_DialogWindows::ExportSequences(1).show();
}

void Polytempo_Composition::exportAllSequences()
{
    if(score == nullptr) return;
    
    exportAll = true;
    Polytempo_DialogWindows::ExportSequences(0).show();
}

void Polytempo_Composition::exportAsPlainText()
{
    File directory(Polytempo_StoredPreferences::getInstance()->getProps().getValue("scoreFileDirectory"));
    FileChooser fileChooser("Export Composition As Plain List", directory, "*.txt", true);
    
    if(fileChooser.browseForFileToSave(true))
    {
        File file = fileChooser.getResult();
        bool firstEvent;
        
        file.replaceWithText("");
        
        if(exportAll)
        {
            for(Polytempo_Sequence *sequence : sequences)
            {
                firstEvent = true;
                
                for(Polytempo_Event *event : sequence->getEvents())
                {
                    if(event->getType() == eventType_Beat)
                    {
                        if(firstEvent) firstEvent = false;
                        else           file.appendText(" ");
                    
                        file.appendText(String(event->getTime() * 0.001f));
                    }
                }
                file.appendText("\n\n");
            }
        }
        else
        {
            Polytempo_Sequence *sequence = getSelectedSequence();
            firstEvent = true;
            
            for(Polytempo_Event *event : sequence->getEvents())
            {
                if(event->getType() == eventType_Beat)
                {
                    if(firstEvent) firstEvent = false;
                    else           file.appendText(" ");
                    
                    file.appendText(String(event->getTime() * 0.001f));
                }
            }
        }

        file.create();
    }
}

void Polytempo_Composition::exportAsLispList()
{
    File directory(Polytempo_StoredPreferences::getInstance()->getProps().getValue("scoreFileDirectory"));
    FileChooser fileChooser("Export Composition As Plain List", directory, "*.txt", true);
    
    if(fileChooser.browseForFileToSave(true))
    {
        File file = fileChooser.getResult();
        bool firstEvent;
        
        file.replaceWithText("");

        if(exportAll)
        {
            file.appendText("(");
            for(Polytempo_Sequence *sequence : sequences)
            {
                firstEvent = true;
                
                file.appendText("(");
                for(Polytempo_Event *event : sequence->getEvents())
                {
                    if(event->getType() == eventType_Beat)
                    {
                        if(firstEvent) firstEvent = false;
                        else           file.appendText(" ");
                        
                        file.appendText(String(event->getTime() * 0.001f));
                    }
                }
                file.appendText(")");
            }
            file.appendText(")");
        }
        else
        {
            Polytempo_Sequence *sequence = getSelectedSequence();
            firstEvent = true;
            
            file.appendText("(");
            for(Polytempo_Event *event : sequence->getEvents())
            {
                if(event->getType() == eventType_Beat)
                {
                    if(firstEvent) firstEvent = false;
                    else           file.appendText(" ");
                    
                    file.appendText(String(event->getTime() * 0.001f));
                }
            }
            file.appendText(")");
        }
        
        file.create();
    }
}

void Polytempo_Composition::exportAsCArray()
{
    File directory(Polytempo_StoredPreferences::getInstance()->getProps().getValue("scoreFileDirectory"));
    FileChooser fileChooser("Export Composition As Plain List", directory, "*.txt", true);
    
    if(fileChooser.browseForFileToSave(true))
    {
        File file = fileChooser.getResult();
        bool firstEvent, firstSequence;
        
        file.replaceWithText("");
        
        if(exportAll)
        {
            firstSequence = true;

            file.appendText("[ ");
            for(Polytempo_Sequence *sequence : sequences)
            {
                firstEvent = true;

                if(firstSequence)   firstSequence = false;
                else                file.appendText(", ");

                file.appendText("[ ");
                for(Polytempo_Event *event : sequence->getEvents())
                {
                    if(event->getType() == eventType_Beat)
                    {
                        if(firstEvent) firstEvent = false;
                        else           file.appendText(", ");
                        
                        file.appendText(String(event->getTime() * 0.001f));
                    }
                }
                file.appendText(" ]");
            }
            file.appendText(" ]");
        }
        else
        {
            Polytempo_Sequence *sequence = getSelectedSequence();
            firstEvent = true;
            
            file.appendText("[ ");
            for(Polytempo_Event *event : sequence->getEvents())
            {
                if(event->getType() == eventType_Beat)
                {
                    if(firstEvent) firstEvent = false;
                    else           file.appendText(", ");
                    
                    file.appendText(String(event->getTime() * 0.001f));
                }
            }
            file.appendText(" ]");
        }
        
        file.create();
    }
}

void Polytempo_Composition::exportAsPolytempoScore()
{
    File directory(Polytempo_StoredPreferences::getInstance()->getProps().getValue("scoreFileDirectory"));
    FileChooser fileChooser("Export Composition As Polytempo Score", directory, "*.ptsco", true);

    if(fileChooser.browseForFileToSave(true))
    {
        // build a score to export
        ScopedPointer < Polytempo_Score > tempScore = new Polytempo_Score();
        Polytempo_Event *tempEvent;
        
        if(exportAll)
        {
            for(int i=0;i<sequences.size();i++)
            {
                tempScore->addSection("sequence"+String(i));
                
                for(Polytempo_Event *event : *score->getEvents())
                {
                    if(event->hasProperty("~sequence") && (int)event->getProperty("~sequence") == i)
                    {
                        tempEvent = new Polytempo_Event(*event);

                        if(tempEvent->getType() == eventType_Beat)
                            tempEvent->removeProperty(eventPropertyString_Value);

                        tempEvent->removeProperty("~sequence");
                        
                        tempScore->addEvent(tempEvent);
                    }
                }

            }
        }
        else
        {
            tempScore->addSection("sequence"+String(selectedSequenceIndex));
            
            for(Polytempo_Event *event : *score->getEvents())
            {
                if(event->hasProperty("~sequence") && (int)event->getProperty("~sequence") == selectedSequenceIndex)
                {
                    tempEvent = new Polytempo_Event(*event);
                    
                    if(tempEvent->getType() == eventType_Beat)
                        tempEvent->removeProperty(eventPropertyString_Value);
                    
                    tempEvent->removeProperty("~sequence");
                    
                    tempScore->addEvent(tempEvent);
                }
            }
        }
        
        // save to file
        File scoreFile = fileChooser.getResult();
        String tempurl(scoreFile.getParentDirectory().getFullPathName());
        File tempFile(tempurl<<"/~temp.ptsco");
        tempScore->writeToFile(tempFile);
        tempFile.copyFileTo(scoreFile);
        tempFile.deleteFile();
        tempScore = nullptr;
    }
}
