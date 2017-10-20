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
    
    updateScore();
}

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
}

void Polytempo_Composition::removeSequence(int index)
{
    sequences.remove(index);
    
    if(selectedSequenceIndex > sequences.size() - 1) selectedSequenceIndex--;

    updateContent();
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
       selectedControlPointIndex == getSelectedSequence()->getControlPoints()->size()-1)
        return false;
    else
        return true;
}

void Polytempo_Composition::updateScore()
{
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
            controlPoint0->coincidingControlPoints->clearQuick();

            int ii = -1;
            while((sequence1 = getSequence(++ii)) != nullptr)
            {
                if(sequence0 == sequence1) continue;
                
                int jj = -1;
                while((controlPoint1 = sequence1->getControlPoint(++jj)) != nullptr)
                {
                    //DBG("i: "<<i<<" j: "<<j<<" ii: "<<ii<<" jj: "<<jj);
                    if(controlPoint0->time == controlPoint1->time)  controlPoint0->coincidingControlPoints->add(controlPoint1);
                }
            }
        }
    }
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
