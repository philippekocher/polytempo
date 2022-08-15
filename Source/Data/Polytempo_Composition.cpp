#include "Polytempo_Composition.h"
#include "../Application/PolytempoComposer/Polytempo_ComposerApplication.h"
#include "../Scheduler/Polytempo_ScoreScheduler.h"
#include "../Scheduler/Polytempo_EventScheduler.h"
#include "../Preferences/Polytempo_StoredPreferences.h"
#include "../Views/PolytempoComposer/Polytempo_DialogWindows.h"
#include "../Audio+Midi/Polytempo_AudioClick.h"


Polytempo_Composition::Polytempo_Composition()
{
    score.reset(new Polytempo_Score());
    Polytempo_ScoreScheduler::getInstance()->setScore(score.get());
    
    selectedControlPointIndices = new Array<int>();
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
    
    Rational maxPos = 0;
    float    maxTime = 0;
    for(Polytempo_Sequence *seq : sequences)
    {
        if(seq->getMaxPosition() > maxPos) maxPos = seq->getMaxPosition();
        if(seq->getMaxTime() > maxTime) maxTime = seq->getMaxTime();
    }
    
    if(mainWindow)
    {
        if(((Polytempo_ComposerWindow*)mainWindow)->getContentID() == 0)
            ((Polytempo_ComposerMainView*)mainWindow->getContentComponent())->setMapDimension(maxTime, maxPos);
        mainWindow->repaint();
        mainWindow->getContentComponent()->resized();
    }
    
    Polytempo_ComposerApplication::getCommandManager().commandStatusChanged(); // update menubar
    Polytempo_AudioClick::getInstance()->setNumVoices(sequences.size());
    
    scoreNeedsUpdate = true;

    // the score mustn't be updated while it is being read by the scheduler
    if(!Polytempo_ScoreScheduler::getInstance()->isRunning()) updateScore();
}

void Polytempo_Composition::setDirty(bool flag) { dirty = flag; }
bool Polytempo_Composition::isDirty() { return dirty; }

void Polytempo_Composition::addSequence()
{
    Polytempo_Sequence *sequence;
    
    sequence = new Polytempo_Sequence(++sequenceCounter);
    
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

Polytempo_Sequence* Polytempo_Composition::getSequenceWithID(int id)
{
    for(Polytempo_Sequence* seq : sequences)
    {
        if(seq->getID() == id) return seq;
    }
    return nullptr;
}


bool Polytempo_Composition::isOneSequenceSoloed()
{
    for(Polytempo_Sequence* seq : sequences)
    {
        if(seq->isSoloed()) return true;
    }
    return false;
}

void Polytempo_Composition::setSelectedSequenceIndex(int i)
{
    selectedSequenceIndex = i;
    clearSelectedControlPointIndices();
}

int Polytempo_Composition::getSelectedSequenceIndex()
{
    return selectedSequenceIndex;
}

void Polytempo_Composition::clearSelectedControlPointIndices()
{
    selectedControlPointIndices->clearQuick();
}

void Polytempo_Composition::addSelectedControlPointIndex(int i)
{
    selectedControlPointIndices->add(i);
}

void Polytempo_Composition::setSelectedControlPointIndex(int i)
{
    clearSelectedControlPointIndices();
    addSelectedControlPointIndex(i);
}

void Polytempo_Composition::removeSelectedControlPointIndex(int i)
{
    int indexToRemove = selectedControlPointIndices->indexOf(i);
    selectedControlPointIndices->remove(indexToRemove);
}

Array<int>* Polytempo_Composition::getSelectedControlPointIndices()
{
    return selectedControlPointIndices;
}

bool Polytempo_Composition::isSelectedControlPointIndex(int i)
{
    return selectedControlPointIndices->contains(i);
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
        score->addEvents(sequence->getTimedEvents());
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
    else if(modalResult == 1) Polytempo_Composition::getInstance()->saveToFile(false);       // yes
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
    Polytempo_EventScheduler::getInstance()->scheduleEvent(Polytempo_Event::makeEvent(eventType_DeleteAll));
    sequenceCounter = 0;

    addSequence(); // one sequence to start with
    setDirty(false);
    
    compositionFile = File();
    mainWindow->setName("Untitled");

}

void Polytempo_Composition::openFile(File file)
{
    if(dirty)
    {
        shouldOpenFile = file;
        unsavedChangesAlert(Polytempo_YesNoCancelAlert::openDocumentTag);
        return;
    }

    if(!file.existsAsFile())
    {
        if(shouldOpenFile.existsAsFile())
        {
            file = shouldOpenFile;
        }
        else
        {
            File directory(Polytempo_StoredPreferences::getInstance()->getProps().getValue("compositionFileDirectory"));
            FileChooser fileChooser("Open Score File", directory, "*.json;*.ptcom", true);
    
            if(!fileChooser.browseForFileToOpen()) return;

            file = fileChooser.getResult();
        }
    }

    if(readJSONfromFile(file))
    {
        compositionFile = file;
        dirty = false;
        
        mainWindow->setName(compositionFile.getFileNameWithoutExtension());
        Polytempo_StoredPreferences::getInstance()->getProps().setValue("compositionFileDirectory", compositionFile.getParentDirectory().getFullPathName());
        Polytempo_StoredPreferences::getInstance()->recentFiles.addFile(compositionFile);
    }
}


void Polytempo_Composition::saveToFile(bool showFileDialog)
{
    if(!compositionFile.existsAsFile() || showFileDialog)
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
            Polytempo_StoredPreferences::getInstance()->recentFiles.addFile(compositionFile);
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
    Polytempo_EventScheduler::getInstance()->scheduleEvent(Polytempo_Event::makeEvent(eventType_DeleteAll));
    sequenceCounter = 0;

    // iterate and addSequence
    DynamicObject* jsonObject;
    for(int i=0; i < jsonSequences.size(); i++)
    {
        Polytempo_Sequence *sequence = new Polytempo_Sequence(++sequenceCounter);
        sequence->setObject(jsonObject = jsonSequences.getValueAt(i).getDynamicObject());
        sequences.add(sequence);
    }
    
    selectedSequenceIndex = 0;
    
    updateContent();
    return true;
}


//------------------------------------------------------------------------------
#pragma mark -
#pragma mark export

#define string_ExportPlainList "Export Composition as Plain List"
#define string_ExportPTSCO     "Export Composition as Polytempo Score"
#define string_ExportAudio     "Export Composition as Audio Click Track"

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
    FileChooser fileChooser(string_ExportPlainList, directory, "*.txt", true);
    
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
                
                for(Polytempo_Event *event : sequence->getTimedEvents())
                {
                    if(event->getType() == eventType_Beat &&
                       event->hasDefinedTime())
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
            
            for(Polytempo_Event *event : sequence->getTimedEvents())
            {
                if(event->getType() == eventType_Beat &&
                   event->hasDefinedTime())
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
    FileChooser fileChooser(string_ExportPlainList, directory, "*.txt", true);
    
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
                for(Polytempo_Event *event : sequence->getTimedEvents())
                {
                    if(event->getType() == eventType_Beat &&
                       event->hasDefinedTime())
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
            for(Polytempo_Event *event : sequence->getTimedEvents())
            {
                if(event->getType() == eventType_Beat &&
                   event->hasDefinedTime())
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
    FileChooser fileChooser(string_ExportPlainList, directory, "*.txt", true);
    
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
                for(Polytempo_Event *event : sequence->getTimedEvents())
                {
                    if(event->getType() == eventType_Beat &&
                       event->hasDefinedTime())
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
            for(Polytempo_Event *event : sequence->getTimedEvents())
            {
                if(event->getType() == eventType_Beat &&
                   event->hasDefinedTime())
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
    FileChooser fileChooser(string_ExportPTSCO, directory, "*.ptsco", true);

    if(fileChooser.browseForFileToSave(true))
    {
        // build a score to export
        Polytempo_Score tempScore;
        Polytempo_Event *tempEvent;
        
        if(exportAll)
        {
            for(Polytempo_Sequence *sequence : sequences)
            {
                tempScore.addSection("sequence"+String(sequence->getID()));
                
                for(Polytempo_Event *event : sequence->getTimedEvents())
                {
                    if(event->hasProperty("~sequence") && (int)event->getProperty("~sequence") == sequence->getID())
                    {
                        tempEvent = new Polytempo_Event(*event);

                        if(!tempEvent->hasDefinedTime()) continue;
                        
                        if(tempEvent->getType() == eventType_Beat)
                            tempEvent->removeProperty(eventPropertyString_Value);

                        tempEvent->removeProperty("~sequence");
                        
                        tempScore.addEvent(tempEvent);
                    }
                }

            }
        }
        else
        {
            Polytempo_Sequence *sequence = getSelectedSequence();
            tempScore.addSection("sequence"+String(sequence->getID()));

            for(Polytempo_Event *event : sequence->getTimedEvents())
            {
                if(event->hasProperty("~sequence") && (int)event->getProperty("~sequence") == sequence->getID())
                {
                    tempEvent = new Polytempo_Event(*event);

                    if(!tempEvent->hasDefinedTime()) continue;

                    if(tempEvent->getType() == eventType_Beat)
                        tempEvent->removeProperty(eventPropertyString_Value);
                    
                    tempEvent->removeProperty("~sequence");
                    
                    tempScore.addEvent(tempEvent);
                }
            }
        }
        
        // save to file
        File scoreFile = fileChooser.getResult();
        String tempurl(scoreFile.getParentDirectory().getFullPathName());
        File tempFile(tempurl<<"/~temp.ptsco");
        tempScore.writeToFile(tempFile);
        tempFile.copyFileTo(scoreFile);
        tempFile.deleteFile();
    }
}

void Polytempo_Composition::exportAsAudio()
{
    File directory(Polytempo_StoredPreferences::getInstance()->getProps().getValue("scoreFileDirectory"));
    FileChooser fileChooser(string_ExportAudio, directory, "*.wav", true);

    if(fileChooser.browseForFileToSave(true))
    {
        int sampleRate = 44100;
        int time, maxTime = 0, minTime = 0;
        int ch = 0;

        for(Polytempo_Sequence *sequence : sequences)
        {
            if(!exportAll && sequence != getSelectedSequence()) continue;
            time = int((sequence->getTimedEvents()[0]->getTime()) * 0.001 * sampleRate);
            minTime = abs(time) > minTime ? abs(time) : minTime;
            time = int((sequence->getMaxTime() + 1) * sampleRate); // 1s added for the duration of the last click
            maxTime = time > maxTime ? time : maxTime;
            ch++;
        }

        FileOutputStream* outputStream = new FileOutputStream(fileChooser.getResult());
        AudioBuffer<float> buffer = AudioBuffer<float>(ch, maxTime + minTime);
        
        ch = 0;
        for(Polytempo_Sequence *sequence : sequences)
        {
            if(!exportAll && sequence != getSelectedSequence()) continue;
            for(Polytempo_Event *event : sequence->getTimedEvents())
            {
                sequence->addPlaybackPropertiesToEvent(event);
                Polytempo_AudioClick::getInstance()->writeClickforEvent(event, buffer, ch, 44100, minTime);
            }
            ch++;
        }
        
        WavAudioFormat format;
        std::unique_ptr<AudioFormatWriter> writer;
        writer.reset(format.createWriterFor(outputStream,
                                            44100.0,
                                            (unsigned int)buffer.getNumChannels(),
                                            24,
                                            {},
                                            0));
        if (writer != nullptr)
            writer->writeFromAudioSampleBuffer(buffer, 0, buffer.getNumSamples());
    }
}
