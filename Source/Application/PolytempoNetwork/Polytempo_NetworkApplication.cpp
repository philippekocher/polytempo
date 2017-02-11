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

#include "Polytempo_NetworkApplication.h"

//#include "../../JuceLibraryCode/JuceHeader.h"
#include "../../Preferences/Polytempo_StoredPreferences.h"
#include "../../Scheduler/Polytempo_Scheduler.h"
#include "../../Scheduler/Polytempo_EventDispatcher.h"
#include "../../Audio+Midi/Polytempo_AudioClick.h"
#include "../../Audio+Midi/Polytempo_MidiClick.h"
#include "../../Views/PolytempoNetwork/Polytempo_ImageManager.h"
#include "../../Network/Polytempo_NetworkSupervisor.h"
#include "../../Misc/Polytempo_Alerts.h"

Polytempo_NetworkApplication::Polytempo_NetworkApplication()
{}

void Polytempo_NetworkApplication::initialise(const String&)
{
    // GUI
    mainWindow = new Polytempo_NetworkWindow();
    
    LookAndFeel::getDefaultLookAndFeel().setUsingNativeAlertWindows(true);
    
    // create and manage a MenuBarComponent
    menuBarModel = new Polytempo_MenuBarModel(mainWindow);
    
    // use keypresses that arrive in the windows to send out commands
    mainWindow->addKeyListener(menuBarModel->commandManager.getKeyMappings());
    
    // scheduler
    Polytempo_Scheduler::getInstance()->setEngine(new Polytempo_NetworkEngine());
    
    // create network connection
    oscListener = new Polytempo_OSCListener(47522);
    oscSender   = new Polytempo_OSCSender();
    oscSender->addBroadcastSender(47522);
    Polytempo_EventDispatcher::getInstance()->setBroadcastSender(oscSender);
    
    // audio and midi
    Polytempo_AudioClick::getInstance();
    Polytempo_MidiClick::getInstance();
    midiInput = new Polytempo_MidiInput();
    
    // image manager
    Polytempo_ImageManager::getInstance();
    
#if (!JUCE_DEBUG)
    // contact web server
    getApplicationName();
    getApplicationVersion();
    URL url = URL("http://polytempo.zhdk.ch/stats/log.php?application="+getApplicationName()+"&version="+getApplicationVersion());
    ScopedPointer<InputStream> stream = url.createInputStream(true);
    if(stream != nullptr)
        DBG(stream->readString());
    stream = nullptr;
#endif
    
    // open default score file
    if(Polytempo_StoredPreferences::getInstance()->getProps().getValue("defaultFilePath") != String::empty)
    {
        scoreFile = *new File(Polytempo_StoredPreferences::getInstance()->getProps().getValue("defaultFilePath"));
        if(scoreFile.exists())
        {
            Polytempo_StoredPreferences::getInstance()->getProps().setValue("scoreFileDirectory", scoreFile.getParentDirectory().getFullPathName());
            Polytempo_Score *newScore = nullptr;
            Polytempo_Score::parse(scoreFile, &newScore);
            if(newScore != nullptr)
            {
                score = newScore;
                Polytempo_Scheduler::getInstance()->setScore(score);
                mainWindow->setName(scoreFile.getFileNameWithoutExtension());
    
                if(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("fullScreen"))
                {
                    mainWindow->setFullScreen(true);
                }
            }
        }
    }
    else
    {
        score = new Polytempo_Score();
        Polytempo_Scheduler::getInstance()->setScore(score);
    }
}

void Polytempo_NetworkApplication::shutdown()
{
    DBG("..shutdown");
    // cleaning-up needed before the app is shut down.
    
    if(mainWindow != nullptr) mainWindow->setMenuBar(0);
    
    // save the current size and position to our settings file..
    Polytempo_StoredPreferences::getInstance()->getProps().setValue("mainWindow", mainWindow->getWindowStateAsString());
    Polytempo_StoredPreferences::getInstance()->getProps().save();
    

    // delete all open windows (except the main window)
    int num = Desktop::getInstance().getNumComponents();
    for(int i=0;i<num;i++)
    {
        Component *c = Desktop::getInstance().getComponent(i);
        if(c != mainWindow) delete c;
    }
    
    // delete scoped pointers
    mainWindow = nullptr;
    oscSender = nullptr;
    oscListener = nullptr;
    midiInput = nullptr;
    score = nullptr;

    // delete singletons
    Polytempo_StoredPreferences::deleteInstance();
    Polytempo_AudioClick::deleteInstance();
    Polytempo_MidiClick::deleteInstance();
    Polytempo_Scheduler::deleteInstance();
    Polytempo_NetworkSupervisor::deleteInstance();
    Polytempo_ImageManager::deleteInstance();
}

//------------------------------------------------------------------------------
void Polytempo_NetworkApplication::systemRequestedQuit()
{
    DBG("systemRequestedQuit");
    applicationShouldQuit();
}

static void unsavedChangesCallback(int modalResult, double customValue)
{
    Polytempo_NetworkApplication* const app = dynamic_cast<Polytempo_NetworkApplication*>(JUCEApplication::getInstance());
    
    if(modalResult == 0)      return;                               // cancel
    else if(modalResult == 1) app->saveScoreFile(false);            // yes
    else if(modalResult == 2) app->getScore()->setDirty(false);     // no


    if(customValue == 0)      app->applicationShouldQuit();
    else if(customValue == 1) app->openScoreFile();
}

void Polytempo_NetworkApplication::applicationShouldQuit()
{
    if(Polytempo_Scheduler::getInstance()->isRunning())
    {
        quitApplication = true;
        Polytempo_Scheduler::getInstance()->stop();
        return;
        
        // after the scheduler and the visual metro will have stopped
        // applicationShouldQuit() will be called again
    }
    
    if(score && score->isDirty())
    {
        DBG("save score");
        String title;
        Polytempo_YesNoCancelAlert::show(title << "Do you want to save the changes to \"" << scoreFile.getFileName().toRawUTF8() << "\"?", "If you don't save your changes will be lost.",ModalCallbackFunction::create(unsavedChangesCallback,0.0));
        return;

        // after the score will have been saved
        // applicationShouldQuit() will be called again
    }
    
    quit();
}

void Polytempo_NetworkApplication::anotherInstanceStarted (const String&)
{
    // When another instance of the app is launched while this one is running,
    // this method is invoked, and the commandLine parameter tells you what
    // the other instance's command-line arguments were.
}

void Polytempo_NetworkApplication::openFileDialog()
{
    File directory(Polytempo_StoredPreferences::getInstance()->getProps().getValue("scoreFileDirectory"));
    FileChooser fileChooser("Open Score File", directory, "*.json;*.ptsco", true);
    
    if(fileChooser.browseForFileToOpen())
    {
        openScoreFile(fileChooser.getResult());
    }
}

void Polytempo_NetworkApplication::saveScoreFile(bool showFileDialog)
{
    if(score == nullptr) return;
    if(scoreFile == File::nonexistent && !showFileDialog) return;
    
    File directory(Polytempo_StoredPreferences::getInstance()->getProps().getValue("scoreFileDirectory"));
    FileChooser fileChooser("Save Score File", scoreFile, "*.ptsco", true);
    
    String tempurl(directory.getFullPathName());
    File tempFile(tempurl<<"/~temp.ptsco");
    
                  
    if(showFileDialog)
    {
        if(fileChooser.browseForFileToSave(true))
        {
            scoreFile = fileChooser.getResult();
            score->writeToFile(tempFile);
            tempFile.copyFileTo(scoreFile);
        }
    }
    else
    {
        score->writeToFile(tempFile);
        tempFile.copyFileTo(scoreFile);
    }
    tempFile.deleteFile();
}

void Polytempo_NetworkApplication::openScoreFilePath(String filePath)
{
    // check if Path exists and open file
    File file = File(filePath);
    if(file.existsAsFile() == 1) openScoreFile(file);
}

void Polytempo_NetworkApplication::openScoreFile(File newScoreFile)
{
    if(newScoreFile != File::nonexistent)
    {
        if(newScoreFile.existsAsFile())
            scoreFile = newScoreFile;
        else
        {
            Polytempo_Alert::show("Error", "File does not exist:\n" + newScoreFile.getFullPathName());
            return;
        }
        
    }
    else if(newScoreFile == scoreFile) return; // file already open

    if(score && score->isDirty())
    {
        DBG("save score");
        String title;
        Polytempo_YesNoCancelAlert::show(title << "Do you want to save the changes to \"" << scoreFile.getFileName().toRawUTF8() << "\"?", "If you don't save your changes will be lost.", ModalCallbackFunction::create(unsavedChangesCallback, 1.0));
        return;
    }
    
    Polytempo_StoredPreferences::getInstance()->getProps().setValue("scoreFileDirectory", scoreFile.getParentDirectory().getFullPathName());
    
    Polytempo_Score *newScore = nullptr;
 
    Polytempo_Score::parse(scoreFile, &newScore);
    // TODO: validate score
    //  - has section "init"?
    
    if(newScore != nullptr)
    {
        DBG("set score");
        score = newScore;
        Polytempo_Scheduler::getInstance()->setScore(score);
        mainWindow->setName(scoreFile.getFileNameWithoutExtension());

        // add to recent files
        Polytempo_StoredPreferences::getInstance()->recentFiles.addFile(scoreFile);
    }    
}

//------------------------------------------------------------------------------
