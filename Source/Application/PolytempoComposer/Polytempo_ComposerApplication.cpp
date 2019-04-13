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
    commandManager = new ApplicationCommandManager();
    commandManager->registerAllCommandsForTarget(this);
    
    // scheduler
    Polytempo_ScoreScheduler::getInstance()->setEngine(new Polytempo_ComposerEngine);
    Polytempo_EventScheduler::getInstance()->startThread(5); // priority between 0 and 10
    
    // audio and midi
    Polytempo_AudioClick::getInstance();
    Polytempo_MidiClick::getInstance();

    // window
    composerWindow = new Polytempo_ComposerWindow();
    
    Polytempo_Composition::getInstance()->setMainWindow(composerWindow);
    Polytempo_Composition::getInstance()->newComposition();

    // return to beginning
    Polytempo_ScoreScheduler::getInstance()->returnToBeginning();
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
