#include "Polytempo_ComposerApplication.h"

#include "../../Preferences/Polytempo_StoredPreferences.h"
#include "../../Audio+Midi/Polytempo_AudioClick.h"
#include "../../Audio+Midi/Polytempo_MidiClick.h"
#include "../../Scheduler/Polytempo_ScoreScheduler.h"
#include "../../Scheduler/Polytempo_EventScheduler.h"
#include "../../Scheduler/Polytempo_EventDispatcher.h"
#include "../../Network/Polytempo_TimeProvider.h"
#include "../../Misc/Polytempo_Alerts.h"


Polytempo_ComposerApplication::Polytempo_ComposerApplication() {}
    
void Polytempo_ComposerApplication::initialise(const String& /*commandLine*/)
{
    commandManager.reset(new ApplicationCommandManager());
    commandManager->registerAllCommandsForTarget(this);
    
    // scheduler
    Polytempo_ScoreScheduler::getInstance()->setEngine(new Polytempo_ComposerEngine);
    Polytempo_EventScheduler::getInstance()->startThread(5); // priority between 0 and 10
    
    // audio and midi
    Polytempo_AudioClick::getInstance();
    Polytempo_MidiClick::getInstance();

    // window
    composerWindow.reset(new Polytempo_ComposerWindow());
    
    Polytempo_Composition::getInstance()->setMainWindow(composerWindow.get());
    Polytempo_Composition::getInstance()->newComposition();

    // return to beginning
    Polytempo_ScoreScheduler::getInstance()->returnToBeginning();
    
#if (! JUCE_DEBUG)
    // contact web server
    URL url = URL("https://polytempo.zhdk.ch/stats/log.php?application="+getApplicationName()+"&version="+getApplicationVersion()+"&os="+SystemStats::getOperatingSystemName()+"&user="+SystemStats::getFullUserName());
    auto stream = url.createInputStream(true);
#endif
}
    
void Polytempo_ComposerApplication::shutdown()
{
    DBG("..shutdown");
    if(composerWindow)
    {
        // save the current properties of the main window
        Polytempo_StoredPreferences::getInstance()->getProps().setValue("mainWindow", composerWindow->getWindowStateAsString());
        Polytempo_StoredPreferences::getInstance()->getProps().setValue("mainWindowContent", composerWindow->getWindowContentStateAsString());
    }
    
    Polytempo_StoredPreferences::getInstance()->getProps().save();
    
    // delete scoped pointers
    composerWindow = nullptr;
    commandManager = nullptr;
    
    // delete singletons
    Polytempo_StoredPreferences::deleteInstance();
    Polytempo_Composition::deleteInstance();
    Polytempo_AudioClick::deleteInstance();
    Polytempo_MidiClick::deleteInstance();
    Polytempo_ScoreScheduler::deleteInstance(); // delete after observers!
    Polytempo_EventScheduler::deleteInstance();
    Polytempo_TimeProvider::deleteInstance();
    Polytempo_EventDispatcher::deleteInstance();
}
    
void Polytempo_ComposerApplication::systemRequestedQuit()
{
    applicationShouldQuit();
}

void Polytempo_ComposerApplication::applicationShouldQuit()
{
    if(Polytempo_ScoreScheduler::getInstance()->isRunning())
    {
        Polytempo_ScoreScheduler::getInstance()->kill();
    }
    
    if(Polytempo_Composition::getInstance()->isDirty())
    {
        Polytempo_Composition::getInstance()->unsavedChangesAlert(Polytempo_YesNoCancelAlert::applicationQuitTag);
        return;

        // after the score will have been saved
        // applicationShouldQuit() will be called again
    }
    
    quit();
}
    
void Polytempo_ComposerApplication::anotherInstanceStarted(const String& /*commandLine*/)
{
    // When another instance of the app is launched while this one is running,
    // this method is invoked, and the commandLine parameter tells you what
    // the other instance's command-line arguments were.
}
    
ApplicationCommandManager& Polytempo_ComposerApplication::getCommandManager()
{
    Polytempo_ComposerApplication* const app = dynamic_cast<Polytempo_ComposerApplication*>(JUCEApplication::getInstance());
    
    return *(app->commandManager);
}
    
DocumentWindow& Polytempo_ComposerApplication::getDocumentWindow()
{
    Polytempo_ComposerApplication* const app = dynamic_cast<Polytempo_ComposerApplication*>(JUCEApplication::getInstance());

    return *(app->composerWindow);
}

Polytempo_ComposerMainView& Polytempo_ComposerApplication::getMainView()
{
    Polytempo_ComposerApplication* const app = dynamic_cast<Polytempo_ComposerApplication*>(JUCEApplication::getInstance());
    
    return *(app->composerWindow->getMainView());
}
