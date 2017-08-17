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

#include "Polytempo_NetworkWindow.h"
#include "../../Views/PolytempoNetwork/Polytempo_NetworkMainView.h"
#include "../../Preferences/Polytempo_StoredPreferences.h"
#include "../../Scheduler/Polytempo_Scheduler.h"


static ScopedPointer<ApplicationCommandManager> applicationCommandManager;

Polytempo_NetworkWindow::Polytempo_NetworkWindow()
    : DocumentWindow (JUCEApplication::getInstance()->getApplicationName(),
                      Colours::lightgrey,
                      DocumentWindow::allButtons)
{
    setUsingNativeTitleBar(true);
    setTitleBarButtonsRequired(5, true); // MAC: green button = maximize instead of fullscreen
    setResizable(true, false);
    setResizeLimits(800, 450, 99999, 99999);
    
    mainView = new Polytempo_NetworkMainView();
    imageEditorView = new Polytempo_ImageEditorView();
    
    // sets the main content component for the window
    setContentNonOwned(mainView, false);

    setBounds(50, 50, 800, 500);
    setVisible(true);

    // restore the last size and position from our settings file...
    restoreWindowStateFromString (Polytempo_StoredPreferences::getInstance()->getProps().getValue ("mainWindow"));
    
    openGLContext.attachTo(*getTopLevelComponent());
    openGLContext.setContinuousRepainting(true);
}

Polytempo_NetworkWindow::~Polytempo_NetworkWindow()
{
    clearContentComponent();
    mainView = nullptr;
    applicationCommandManager = nullptr;

    openGLContext.detach();
}

void Polytempo_NetworkWindow::closeButtonPressed()
{
    JUCEApplication::getInstance()->systemRequestedQuit();
}

void Polytempo_NetworkWindow::setContentID(contentID newContentID)
{
    if(newContentID != currentContentID)
    {
        currentContentID = newContentID;
        
        if(currentContentID == imageEditorViewID)
        {
            imageEditorView->refresh();
            setContentNonOwned(imageEditorView, false);
        }
        else
        {
            setContentNonOwned(mainView, false);
            
            // apply all changes that should be visible in the main view
            Polytempo_Scheduler::getInstance()->executeInit();
            Polytempo_Scheduler::getInstance()->returnToLocator();
        }
    }
}

int Polytempo_NetworkWindow::getContentID()
{
    return currentContentID;
}

Component* Polytempo_NetworkWindow::getContentComponent()
{
    if(currentContentID == mainViewID) return mainView;
    if(currentContentID == imageEditorViewID)  return imageEditorView;
    
    return nullptr;
}
