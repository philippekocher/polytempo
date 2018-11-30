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
#include "../../Application/PolytempoNetwork/Polytempo_NetworkApplication.h"
#include "../../Views/PolytempoNetwork/Polytempo_NetworkMainView.h"
#include "../../Preferences/Polytempo_StoredPreferences.h"
#include "../../Scheduler/Polytempo_ScoreScheduler.h"
#include "../../Misc/Polytempo_Alerts.h"


Polytempo_NetworkWindow::Polytempo_NetworkWindow()
    : DocumentWindow (JUCEApplication::getInstance()->getApplicationName(),
                      Colours::lightgrey,
                      DocumentWindow::allButtons)
{
    setUsingNativeTitleBar(true);
    setTitleBarButtonsRequired(7, true);
    setResizable(true, true);
    setResizeLimits(800, 450, 99999, 99999);

    mainView = new Polytempo_NetworkMainView();
    pageEditorView = new Polytempo_PageEditorView();
    regionEditorView = new Polytempo_RegionEditorView();
    scoreEditorView = new Polytempo_ScoreEditorView();

    // sets the main content component for the window
    setContentNonOwned(mainView, false);

#ifdef JUCE_ANDROID
    setFullScreen(true);
#else
    setBounds(50, 50, 800, 500);
#endif

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

    openGLContext.detach();
}

void Polytempo_NetworkWindow::closeButtonPressed()
{
    JUCEApplication::getInstance()->systemRequestedQuit();
}

static void saveOkCancelCallback (int result, Polytempo_NetworkWindow* pParent)
{
    if(result)
    {
        Polytempo_NetworkApplication *const app = dynamic_cast<Polytempo_NetworkApplication *>(JUCEApplication::getInstance());
        app->saveScoreFile(true);
        if(app->scoreFileExists())
        {
            pParent->performSetContentID();
        }
    }
}

void Polytempo_NetworkWindow::setContentID(contentID newContentID) {
    if (newContentID != currentContentID) {
        Polytempo_NetworkApplication *const app = dynamic_cast<Polytempo_NetworkApplication *>(JUCEApplication::getInstance());

        contentIdToBeSet = newContentID;
        if (newContentID == pageEditorViewID && !app->scoreFileExists())
        {
            Polytempo_OkCancelAlert::show("Save?",
                                          "You must first save the document before you can access the Page Editor",
                                          ModalCallbackFunction::create(saveOkCancelCallback, this));
        }
        else if (newContentID == regionEditorViewID && !app->scoreFileExists())
        {
            Polytempo_OkCancelAlert::show("Save?",
                                          "You must first save the document before you can access the Region Editor",
                                          ModalCallbackFunction::create(saveOkCancelCallback, this));
        }
        else if (newContentID == scoreEditorViewID && !app->scoreFileExists())
        {
            Polytempo_OkCancelAlert::show("Save?",
                                          "You must first save the document before you can access the Score Editor",
                                          ModalCallbackFunction::create(saveOkCancelCallback, this));
        }
        else
            performSetContentID();
    }
}

void Polytempo_NetworkWindow::performSetContentID() {
    Polytempo_NetworkApplication *const app = dynamic_cast<Polytempo_NetworkApplication *>(JUCEApplication::getInstance());
    if (!app->scoreFileExists()) // the user has aborted the save process
        return;

    currentContentID = contentIdToBeSet;

    if (currentContentID == pageEditorViewID)
    {
        pageEditorView->refresh();
        setContentNonOwned(pageEditorView, false);
    }
    else if (currentContentID == regionEditorViewID)
    {
        regionEditorView->refresh();
        setContentNonOwned(regionEditorView, false);
    }
    else if (currentContentID == scoreEditorViewID)
    {
        setContentNonOwned(scoreEditorView, false);
    }
    else
    {
        setContentNonOwned(mainView, false);

        // apply all changes that should be visible in the main view
        Polytempo_ScoreScheduler::getInstance()->executeInit();
        Polytempo_ScoreScheduler::getInstance()->returnToLocator();
    }
}

int Polytempo_NetworkWindow::getContentID()
{
    return currentContentID;
}

Component* Polytempo_NetworkWindow::getContentComponent()
{
    if(currentContentID == mainViewID) return mainView;
    if(currentContentID == pageEditorViewID)  return pageEditorView;
    if(currentContentID == regionEditorViewID)  return regionEditorView;
    if(currentContentID == scoreEditorViewID)  return scoreEditorView;

    return nullptr;
}
