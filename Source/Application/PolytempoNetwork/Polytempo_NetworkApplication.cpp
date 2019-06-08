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
#include "../../Views/PolytempoNetwork/Polytempo_GraphicsAnnotationManager.h"
#include "../../Network/Polytempo_TimeProvider.h"
#include "../../Network/Polytempo_InterprocessCommunication.h"

Polytempo_NetworkApplication::Polytempo_NetworkApplication()
{}

void Polytempo_NetworkApplication::initialise(const String&)
{
	fileLogger = FileLogger::createDefaultAppLogger("Polytempo Network", "appLog.log", "Polytemp Network Logfile", 10 * 1024 * 1024);
	Logger::setCurrentLogger(fileLogger);

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
	broadcastWrapper = new Polytempo_BroadcastWrapper(OSC_PORT_COMMUNICATION);
    oscListener = new Polytempo_OSCListener(OSC_PORT_COMMUNICATION);
    Polytempo_EventDispatcher::getInstance()->setBroadcastSender(broadcastWrapper);
	Polytempo_NetworkSupervisor::getInstance()->setBroadcastSender(broadcastWrapper);
	Polytempo_NetworkSupervisor::getInstance()->createSocket(OSC_PORT_COMMUNICATION);
	Polytempo_InterprocessCommunication::getInstance()->toggleMaster(false);

    // audio and midi
    Polytempo_AudioClick::getInstance();
    Polytempo_MidiClick::getInstance();
    midiInput = new Polytempo_MidiInput();
    
    // image manager
    Polytempo_ImageManager::getInstance();
    
	// time sync
	Polytempo_TimeProvider::getInstance()->initialize(OSC_PORT_TIME_SYNC);

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
    if(Polytempo_StoredPreferences::getInstance()->getProps().getValue("defaultFilePath") != String())
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
    oscListener = nullptr;
	broadcastWrapper = nullptr;
    midiInput = nullptr;
    score = nullptr;
	menuBarModel = nullptr;

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
	Polytempo_InterprocessCommunication::deleteInstance();

	Logger::setCurrentLogger(nullptr);
	fileLogger = nullptr;
}

//------------------------------------------------------------------------------
void Polytempo_NetworkApplication::systemRequestedQuit()
{
    DBG("systemRequestedQuit");
    applicationShouldQuit();
}

static void unsavedChangesCallback(int modalResult, Polytempo_YesNoCancelAlert::callbackTag tag)
{
    Polytempo_NetworkApplication* const app = dynamic_cast<Polytempo_NetworkApplication*>(JUCEApplication::getInstance());
    
    if(modalResult == 0)      return;                               // cancel
    else if(modalResult == 1) app->saveScoreFile(false);            // yes
    else if(modalResult == 2) app->getScore()->setDirty(false);     // no


    if     (tag == Polytempo_YesNoCancelAlert::applicationQuitTag) app->applicationShouldQuit();
    else if(tag == Polytempo_YesNoCancelAlert::openDocumentTag)    app->openScoreFile();
    else if(tag == Polytempo_YesNoCancelAlert::newDocumentTag)     app->newScore();
}

void Polytempo_NetworkApplication::applicationShouldQuit()
{
    if(!mainWindow->applyChanges()) // in case there are any pending changes
        return;

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
        unsavedChangesAlert(Polytempo_YesNoCancelAlert::applicationQuitTag);
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

void Polytempo_NetworkApplication::unsavedChangesAlert(Polytempo_YesNoCancelAlert::callbackTag tag)
{
    String title, fileName;
    
    if(scoreFileExists()) fileName = scoreFile.getFileName();
    else                  fileName = mainWindow->getName();

    Polytempo_YesNoCancelAlert::show(title << "Do you want to save the changes to \"" << fileName << "\"?", "If you don't save your changes will be lost.", ModalCallbackFunction::create(unsavedChangesCallback, tag));
}

void Polytempo_NetworkApplication::newScore()
{    
    if(!mainWindow->applyChanges()) // in case there are any pending changes
        return;
    
    if(score && score->isDirty())
    {
        unsavedChangesAlert(Polytempo_YesNoCancelAlert::newDocumentTag);
        return;
    }
    
    score = new Polytempo_Score();
    score->addSection("sequence");
    Polytempo_ScoreScheduler::getInstance()->setScore(score);
    
    mainWindow->setName("Untitled");
}

void Polytempo_NetworkApplication::openFileDialog()
{
    File directory(Polytempo_StoredPreferences::getInstance()->getProps().getValue("scoreFileDirectory"));
#ifdef JUCE_ANDROID
	fc.reset(new FileChooser("Open Score File", directory, "*.json;*.ptsco", true));
	fc->launchAsync(FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles,
		[this](const FileChooser& chooser)
	{
		File result = chooser.getResult();
		openScoreFile(result);
	});
#else
    
#ifdef JUCE_IOS
    FileChooser fileChooser("Open Score File", directory, "*", true);
#else
	FileChooser fileChooser("Open Score File", directory, "*.json;*.ptsco", true);
#endif
    
    if(fileChooser.browseForFileToOpen())
    {
        openScoreFile(fileChooser.getResult());
    }
#endif
}

void Polytempo_NetworkApplication::saveAs(File targetFile)
{
    File directory(Polytempo_StoredPreferences::getInstance()->getProps().getValue("scoreFileDirectory"));
    
    String tempurl(directory.getFullPathName());
    File tempFile(tempurl<<"/~temp.ptsco");

    scoreFile = targetFile;
    score->writeToFile(tempFile);
    tempFile.copyFileTo(scoreFile);
    tempFile.deleteFile();
    mainWindow->setName(scoreFile.getFileNameWithoutExtension());

    // add to recent files
    Polytempo_StoredPreferences::getInstance()->recentFiles.addFile(scoreFile);
}

void Polytempo_NetworkApplication::saveScoreFile(bool showFileDialog)
{
    if(score == nullptr) return;
    if(!scoreFile.exists()) showFileDialog = true;

    if(!mainWindow->applyChanges()) // in case there are any pending changes
        return;

    if(showFileDialog)
    {
#ifdef JUCE_ANDROID
        fc.reset(new FileChooser("Save Score File", scoreFile, "*.ptsco", true));
        fc->launchAsync(FileBrowserComponent::saveMode | FileBrowserComponent::canSelectFiles | FileBrowserComponent::warnAboutOverwriting,
                        [this](const FileChooser& chooser)
                        {
                            File result = chooser.getResult();
                            saveAs(result);
                        });
#else
#ifdef JUCE_IOS
        FileChooser fileChooser("Save Score File", scoreFile, "*", true);
#else
        FileChooser fileChooser("Save Score File", scoreFile, "*.ptsco", true);
#endif
        if(fileChooser.browseForFileToSave(true))
        {
            Polytempo_StoredPreferences::getInstance()->getProps().setValue("scoreFileDirectory", fileChooser.getResult().getParentDirectory().getFullPathName());

            saveAs(fileChooser.getResult());
        }
#endif
    }
    else
    {
        saveAs(scoreFile);
    }
}

void Polytempo_NetworkApplication::openScoreFilePath(String filePath)
{
    // check if Path exists and open file
    File file = File(filePath);
    if(file.existsAsFile() == 1) openScoreFile(file);
}

void Polytempo_NetworkApplication::openScoreFile(File aFile)
{
    if(aFile.exists()) newScoreFile = aFile;
    
    if(!newScoreFile.existsAsFile())
    {
        Polytempo_Alert::show("Error", "File does not exist:\n" + newScoreFile.getFullPathName());
        return;
    }

    if(score && score->isDirty())
    {
        unsavedChangesAlert(Polytempo_YesNoCancelAlert::openDocumentTag);
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
