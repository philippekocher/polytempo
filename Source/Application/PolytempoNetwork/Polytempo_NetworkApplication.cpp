#include "Polytempo_NetworkApplication.h"
#include "../Polytempo_LatestVersionChecker.h"
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
#include "../../Network/Polytempo_PortDefinition.h"

Polytempo_NetworkApplication::Polytempo_NetworkApplication()
{
}

void Polytempo_NetworkApplication::initialise(const String& commandLine)
{
    fileLogger.reset(FileLogger::createDefaultAppLogger("Polytempo Network", "appLog.log", "Polytemp Network Logfile", 10 * 1024 * 1024));
    Logger::setCurrentLogger(fileLogger.get());

    // GUI
    mainWindow.reset(new Polytempo_NetworkWindow());

    // look and feel
    lookAndFeelV3.setUsingNativeAlertWindows(true);
    Desktop::getInstance().setDefaultLookAndFeel(&lookAndFeelV3);

    // create and manage a MenuBarComponent
    menuBarModel.reset(new Polytempo_MenuBarModel(mainWindow.get()));

    // use keypresses that arrive in the windows to send out commands
    mainWindow->addKeyListener(commandManager.getKeyMappings());

    // scheduler
    Polytempo_ScoreScheduler::getInstance()->setEngine(new Polytempo_NetworkEngine());
    Polytempo_EventScheduler::getInstance()->startThread(Thread::Priority::high);

    // create network connection
    oscListener.reset(new Polytempo_OSCListener(POLYTEMPO_NETWORK_PORT_APP));
    Polytempo_NetworkSupervisor::getInstance()->createSender(Polytempo_AdvertisePortCount, Polytempo_AdvertisePorts);
    oscSender.reset(new Polytempo_OSCSender());

    // audio and midi
    Polytempo_AudioClick::getInstance();
    Polytempo_MidiClick::getInstance();
    midiInput.reset(new Polytempo_MidiInput());
    audioPlayer.reset(new Polytempo_AudioPlayer());

    // image manager
    Polytempo_ImageManager::getInstance();

    // time sync
    Polytempo_TimeProvider::getInstance();
    
    // check for updates
    if(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("checkForNewVersion"))
        Polytempo_LatestVersionChecker::getInstance()->checkForNewVersion(false);

#if (! JUCE_DEBUG)
    // contact web server
    URL url = URL("https://polytempo.zhdk.ch/stats/log.php?application="+getApplicationName()+"&version="+getApplicationVersion()+"&os="+SystemStats::getOperatingSystemName()+"&user="+SystemStats::getFullUserName());
    auto stream = url.createInputStream(URL::InputStreamOptions(URL::ParameterHandling::inPostData));
#endif
    
    if (File::isAbsolutePath(commandLine.unquoted()))
    {
        openScoreFilePath(commandLine.unquoted()); // enable 'open with' (WIN)
    }
    // open default score file
    else if (Polytempo_StoredPreferences::getInstance()->getProps().getValue("defaultFilePath") != String())
    {
        File defaultFile = *new File(Polytempo_StoredPreferences::getInstance()->getProps().getValue("defaultFilePath"));
        if (defaultFile.exists())
        {
            openScoreFile(defaultFile);

            if (Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("fullScreen"))
                mainWindow->setFullScreen(true);
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

    if (mainWindow != nullptr) mainWindow->setMenuBar(nullptr);

    // save the current size and position to our settings file..
    if(mainWindow != nullptr)
        Polytempo_StoredPreferences::getInstance()->getProps().setValue("mainWindow", mainWindow->getWindowStateAsString());
    Polytempo_StoredPreferences::getInstance()->getProps().save();

    // delete all open windows (except the main window)
    int num = Desktop::getInstance().getNumComponents();
    for (int i = 0; i < num; i++)
    {
        Component* c = Desktop::getInstance().getComponent(i);
        if (c != mainWindow.get()) delete c;
    }

    // delete scoped pointers
    mainWindow = nullptr;
    oscListener = nullptr;
    oscSender = nullptr;
    midiInput = nullptr;
    audioPlayer = nullptr;
    score = nullptr;
    menuBarModel = nullptr;

    // delete singletons
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
    Polytempo_StoredPreferences::deleteInstance();

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

    if (modalResult == 0) return; // cancel
    else if (modalResult == 1) app->saveScoreFile(false); // yes
    else if (modalResult == 2) app->getScore()->setDirty(false); // no

    if (tag == Polytempo_YesNoCancelAlert::applicationQuitTag) app->applicationShouldQuit();
    else if (tag == Polytempo_YesNoCancelAlert::openDocumentTag) app->openScoreFile();
    else if (tag == Polytempo_YesNoCancelAlert::newDocumentTag) app->newScore();
}

void Polytempo_NetworkApplication::applicationShouldQuit()
{
    if (!mainWindow->applyChanges()) // in case there are any pending changes
        return;

    if (Polytempo_ScoreScheduler::getInstance()->isRunning())
    {
        quitApplication = true;
        Polytempo_ScoreScheduler::getInstance()->stop();
        return;

        // after the scheduler and the visual metro will have stopped
        // applicationShouldQuit() will be called again
    }

    if (score && score->isDirty())
    {
        unsavedChangesAlert(Polytempo_YesNoCancelAlert::applicationQuitTag);
        return;

        // after the score will have been saved
        // applicationShouldQuit() will be called again
    }

    quit();
}

void Polytempo_NetworkApplication::anotherInstanceStarted(const String& commandLine)
{
    openScoreFilePath(commandLine.unquoted()); // enable 'open with' (MAC)
    
    // When another instance of the app is launched while this one is running,
    // this method is invoked, and the commandLine parameter tells you what
    // the other instance's command-line arguments were.
}

void Polytempo_NetworkApplication::unsavedChangesAlert(Polytempo_YesNoCancelAlert::callbackTag tag)
{
    String title;
    String fileName = scoreFileExists() ? scoreFile.getFileName() : mainWindow->getName();

    Polytempo_YesNoCancelAlert::show(title << "Do you want to save the changes to \"" << fileName << "\"?", "If you don't save your changes will be lost.", ModalCallbackFunction::create(unsavedChangesCallback, tag));
}

void Polytempo_NetworkApplication::newScore()
{
    if (!mainWindow->applyChanges()) // in case there are any pending changes
        return;

    if (score && score->isDirty())
    {
        unsavedChangesAlert(Polytempo_YesNoCancelAlert::newDocumentTag);
        return;
    }

    score.reset(new Polytempo_Score());
    score->addSection("sequence");
    Polytempo_ScoreScheduler::getInstance()->setScore(score.get());
    
    scoreFile = File();

    mainWindow->setName("Untitled");

    // update annotations
    Polytempo_GraphicsAnnotationManager::getInstance()->initialize(String(), String());
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

    if (fileChooser.browseForFileToOpen())
    {
        openScoreFile(fileChooser.getResult());
    }
#endif
}

void Polytempo_NetworkApplication::saveAs(File targetFile)
{
    File directory(Polytempo_StoredPreferences::getInstance()->getProps().getValue("scoreFileDirectory"));

    String tempurl(directory.getFullPathName());
    File tempFile(tempurl << "/~temp.ptsco");

    scoreFile = targetFile;
    score->writeToFile(tempFile);
    tempFile.copyFileTo(scoreFile);
    tempFile.deleteFile();
    mainWindow->setName(scoreFile.getFileNameWithoutExtension());

    // add to recent files
    Polytempo_StoredPreferences::getInstance()->recentFiles.addFile(scoreFile);

    String name = scoreFile.getFileNameWithoutExtension();
    Polytempo_GraphicsAnnotationManager::getInstance()->initialize(scoreFile.getParentDirectory().getFullPathName(), name);
}

void Polytempo_NetworkApplication::saveScoreFile(bool showFileDialog)
{
    if (score == nullptr) return;
    if (!scoreFile.exists()) showFileDialog = true;

    if (!mainWindow->applyChanges()) // in case there are any pending changes
        return;

    if (showFileDialog)
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
        if (fileChooser.browseForFileToSave(true))
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
    if (file.existsAsFile() == 1) openScoreFile(file);
}

void Polytempo_NetworkApplication::openScoreFile(File aFile)
{
    if (aFile.exists()) newScoreFile = aFile;

    if (!newScoreFile.existsAsFile())
    {
        Polytempo_Alert::show("Error", "File does not exist:\n" + newScoreFile.getFullPathName());
        return;
    }

    if (score && score->isDirty())
    {
        unsavedChangesAlert(Polytempo_YesNoCancelAlert::openDocumentTag);
        return;
    }

    if (score && !score->isReady())
    {
        DBG("Can't open file, still loading the previous one...");
        Polytempo_EventScheduler::getInstance()->scheduleInitEvent(Polytempo_Event::makeEvent(eventType_Ready));
        return;
    }

    Polytempo_StoredPreferences::getInstance()->getProps().setValue("scoreFileDirectory", newScoreFile.getParentDirectory().getFullPathName());

    Polytempo_Score* newScore = nullptr;

    Polytempo_Score::parse(newScoreFile, &newScore);
    // TODO: validate score
    //  - has section "init"?

    if (newScore != nullptr)
    {
        // load annotations
        String name = newScoreFile.getFileNameWithoutExtension();
        Polytempo_GraphicsAnnotationManager::getInstance()->initialize(newScoreFile.getParentDirectory().getFullPathName(), name);

        DBG("set score");
        scoreFile = newScoreFile;
        score.reset(newScore);
        Polytempo_ScoreScheduler::getInstance()->setScore(score.get());
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
