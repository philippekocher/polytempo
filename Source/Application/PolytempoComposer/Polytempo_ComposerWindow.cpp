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


#include "Polytempo_ComposerWindow.h"
#include "Polytempo_ComposerMenuBarModel.h"
#include "Polytempo_ComposerApplication.h"
#include "../../Preferences/Polytempo_StoredPreferences.h"

//static ScopedPointer<ApplicationCommandManager> applicationCommandManager;

Polytempo_ComposerWindow::Polytempo_ComposerWindow()
: DocumentWindow (JUCEApplication::getInstance()->getApplicationName(),
                  Colours::lightgrey,
                  DocumentWindow::allButtons)
{
    setUsingNativeTitleBar(true);
    setContentOwned(mainView = new Polytempo_ComposerMainView(), true);
    
    setVisible(true);
    setResizable(true,true);
    
    setBounds(50, 50, 800, 500);
    setResizeLimits(500, 400, 5000, 5000);

    // restore the window's properties from the settings file
    restoreWindowStateFromString(Polytempo_StoredPreferences::getInstance()->getProps().getValue("mainWindow"));
    restoreWindowContentStateFromString(Polytempo_StoredPreferences::getInstance()->getProps().getValue("mainWindowContent"));

    // create and manage a MenuBarComponent
	menuBarModel = new Polytempo_ComposerMenuBarModel();
#if !JUCE_MAC
    setMenuBar(menuBarModel);
#endif
    
    // use keypresses that arrive in this window to send out commands
    ApplicationCommandManager* commandManager = &Polytempo_ComposerApplication::getCommandManager();
    addKeyListener(commandManager->getKeyMappings());
    
    LookAndFeel::setDefaultLookAndFeel (&lookAndFeel);
}

Polytempo_ComposerWindow::~Polytempo_ComposerWindow()
{
	setMenuBar(nullptr);
    mainView = nullptr;
}

void Polytempo_ComposerWindow::closeButtonPressed()
{
    // This is called when the user tries to close this window. Here, we'll just
    // ask the app to quit when this happens, but you can change this to do
    // whatever you need.
    JUCEApplication::getInstance()->systemRequestedQuit();
}

String Polytempo_ComposerWindow::getWindowContentStateAsString()
{
    return mainView->getComponentStateAsString();
}

void Polytempo_ComposerWindow::restoreWindowContentStateFromString(const String& string)
{
    mainView->restoreComponentStateFromString(string);
}
