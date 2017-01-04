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
#include "../../Scheduler/Polytempo_Scheduler.h"
#include "../../Network/Polytempo_NetworkSupervisor.h"


Polytempo_ComposerApplication::Polytempo_ComposerApplication() {}
    
void Polytempo_ComposerApplication::initialise(const String& commandLine)
{
    commandManager = new ApplicationCommandManager();
    commandManager->registerAllCommandsForTarget(this);
    
    // scheduler
    Polytempo_Scheduler::getInstance()->setEngine(new Polytempo_ComposerEngine);
    
    // create network connection
    //oscListener = new Polytempo_OSCListener(47522);
    //oscListener->startThread();
    oscSender   = new Polytempo_OSCSender();
    //oscSender->addBroadcastSender(47522);
    //Polytempo_EventDispatcher::getInstance()->setBroadcastSender(oscSender);
    
    // audio and midi
    Polytempo_AudioClick::getInstance();
    Polytempo_MidiClick::getInstance();

    // window
    composerWindow = new Polytempo_ComposerWindow();
    
    Polytempo_Composition::getInstance()->addSequence(); // one sequence to start with
    Polytempo_Composition::getInstance()->setMainWindow(composerWindow);

    // return to beginning
    Polytempo_Scheduler::getInstance()->returnToBeginning();
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
    oscSender = nullptr;
    
    // delete singletons
    Polytempo_StoredPreferences::deleteInstance();
    Polytempo_Composition::deleteInstance();
    Polytempo_AudioClick::deleteInstance();
    Polytempo_MidiClick::deleteInstance();
    Polytempo_NetworkSupervisor::deleteInstance();
    Polytempo_Scheduler::deleteInstance(); // delete after observers!
}
    
void Polytempo_ComposerApplication::systemRequestedQuit()
{
    // This is called when the app is being asked to quit: you can ignore this
    // request and let the app carry on running, or call quit() to allow the app to close.
    quit();
}
    
void Polytempo_ComposerApplication::anotherInstanceStarted(const String& commandLine)
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
    
//DocumentWindow& Polytempo_ComposerApplication::getDocumentWindow()
//{
//    Polytempo_ComposerApplication* const app = dynamic_cast<Polytempo_ComposerApplication*>(JUCEApplication::getInstance());
//
//    return *(app->composerWindow);
//}

Polytempo_ComposerMainView& Polytempo_ComposerApplication::getMainView()
{
    Polytempo_ComposerApplication* const app = dynamic_cast<Polytempo_ComposerApplication*>(JUCEApplication::getInstance());
    
    return *(app->composerWindow->getMainView());
}
