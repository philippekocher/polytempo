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

#include "../../Preferences/Polytempo_StoredPreferences.h"
#include "../../Scheduler/Polytempo_ScoreScheduler.h"
#include "../../Scheduler/Polytempo_EventScheduler.h"
#include "../../Scheduler/Polytempo_EventDispatcher.h"
#include "../../Audio+Midi/Polytempo_AudioClick.h"
#include "../../Audio+Midi/Polytempo_MidiClick.h"
#include "../../Views/PolytempoNetwork/Polytempo_ImageManager.h"
#include "../../Network/Polytempo_NetworkSupervisor.h"
#include "../../Misc/Polytempo_Alerts.h"
#include "../../Views/PolytempoNetwork/Polytempo_GraphicsAnnotationManager.h"
#include "../../Network/Polytempo_TimeProvider.h"

Polytempo_NetworkApplication::Polytempo_NetworkApplication()
{}

void Polytempo_NetworkApplication::initialise(const String&)
{
    // GUI
    mainWindow = new Polytempo_NetworkWindow();
    
    // look and feel
    lookAndFeelV3.setUsingNativeAlertWindows(true);
    Desktop::getInstance().setDefaultLookAndFeel(&lookAndFeelV3);
    
    // create and manage a MenuBarComponent
    menuBarModel = new Polytempo_MenuBarModel(mainWindow);
    
    // use keypresses that arrive in the windows to send out commands
    mainWindow->addKeyListener(commandManager.getKeyMappings());
    
    // scheduler
    Polytempo_ScoreScheduler::getInstance()->setEngine(new Polytempo_NetworkEngine());
    Polytempo_EventScheduler::getInstance()->startThread(5); // priority between 0 and 10
    
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
    
	// time sync
	Polytempo_TimeProvider::getInstance()->initialize(false, TIME_SYNC_OSC_PORT);

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
                Polytempo_ScoreScheduler::getInstance()->setScore(score);
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
        newScore();
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
    Polytempo_NetworkSupervisor::deleteInstance();
    Polytempo_ImageManager::deleteInstance();
    Polytempo_ScoreScheduler::deleteInstance();
    Polytempo_EventScheduler::deleteInstance();
	Polytempo_GraphicsAnnotationManager::deleteInstance();
	Polytempo_TimeProvider::deleteInstance();
    Polytempo_EventDispatcher::deleteInstance();
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
    else if(customValue == 2) app->newScore();

}

void Polytempo_NetworkApplication::applicationShouldQuit()
{
    if(Polytempo_ScoreScheduler::getInstance()->isRunning())
    {
        quitApplication = true;
        Polytempo_ScoreScheduler::getInstance()->stop();
        return;
        
        // after the scheduler and the visual metro will have stopped
        // applicationShouldQuit() will be called again
    }
    
    if(score && score->isDirty())
    {
        unsavedChangesAlert(0.0);
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

void Polytempo_NetworkApplication::unsavedChangesAlert(double customValue)
{
    String title;

    Polytempo_YesNoCancelAlert::show(title << "Do you want to save the changes to \"" << scoreFile.getFileName().toRawUTF8() << "\"?", "If you don't save your changes will be lost.", ModalCallbackFunction::create(unsavedChangesCallback, customValue));
}

void Polytempo_NetworkApplication::newScore()
{
    if(score && score->isDirty())
    {
        unsavedChangesAlert(2.0);
        return;
    }
    
    score = new Polytempo_Score();
    Polytempo_ScoreScheduler::getInstance()->setScore(score);
    
    mainWindow->setName("Untitled");
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
    if(scoreFile == File::nonexistent) showFileDialog = true;
    
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
    mainWindow->setName(scoreFile.getFileNameWithoutExtension());
}

void Polytempo_NetworkApplication::openScoreFilePath(String filePath)
{
    // check if Path exists and open file
    File file = File(filePath);
    if(file.existsAsFile() == 1) openScoreFile(file);
}

void Polytempo_NetworkApplication::openScoreFile(File aFile)
{
    if(aFile != File::nonexistent) newScoreFile = aFile;
    
    if(newScoreFile == scoreFile) return; // file already open
    if(!newScoreFile.existsAsFile())
    {
        Polytempo_Alert::show("Error", "File does not exist:\n" + newScoreFile.getFullPathName());
        return;
    }

    if(score && score->isDirty())
    {
        unsavedChangesAlert(1.0);
        return;
    }
    
    Polytempo_StoredPreferences::getInstance()->getProps().setValue("scoreFileDirectory", newScoreFile.getParentDirectory().getFullPathName());
    
    Polytempo_Score *newScore = nullptr;
 
    Polytempo_Score::parse(newScoreFile, &newScore);
    // TODO: validate score
    //  - has section "init"?
    
    if(newScore != nullptr)
    {
		// load annotations
		String name = newScoreFile.getFileNameWithoutExtension();    	
		Polytempo_GraphicsAnnotationManager::getInstance()->initialize(newScoreFile.getParentDirectory().getFullPathName(), name);

        DBG("set score");
        scoreFile = newScoreFile;
        score = newScore;
        Polytempo_ScoreScheduler::getInstance()->setScore(score);
        mainWindow->setName(scoreFile.getFileNameWithoutExtension());

		
        // add to recent files
        Polytempo_StoredPreferences::getInstance()->recentFiles.addFile(scoreFile);
    }    
}

void Polytempo_NetworkApplication::commandStatusChanged()
{
    commandManager.commandStatusChanged();
}

ApplicationCommandManager* Polytempo_NetworkApplication::getCommandManager()
{
    Polytempo_NetworkApplication* const app = dynamic_cast<Polytempo_NetworkApplication*>(JUCEApplication::getInstance());
    
    return &(app->commandManager);
}

//------------------------------------------------------------------------------
