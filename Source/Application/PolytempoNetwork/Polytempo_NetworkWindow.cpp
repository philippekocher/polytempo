#include "Polytempo_NetworkWindow.h"
#include "../../Application/PolytempoNetwork/Polytempo_NetworkApplication.h"
#include "../../Views/PolytempoNetwork/Polytempo_NetworkMainView.h"
#include "../../Preferences/Polytempo_StoredPreferences.h"
#include "../../Scheduler/Polytempo_ScoreScheduler.h"
#include "../../Misc/Polytempo_Alerts.h"

Polytempo_NetworkWindow::Polytempo_NetworkWindow() : DocumentWindow(JUCEApplication::getInstance()->getApplicationName(),
                                                                    Colours::lightgrey,
                                                                    allButtons)
{
    setUsingNativeTitleBar(true);
    setTitleBarButtonsRequired(7, true);
    setResizable(true, true);
    setResizeLimits(800, 450, 99999, 99999);

    mainView.reset(new Polytempo_NetworkMainView());
    pageEditorView.reset(new Polytempo_PageEditorView());
    regionEditorView.reset(new Polytempo_RegionEditorView());
    scoreEditorView.reset(new Polytempo_ScoreEditorView());

    // sets the main content component for the window
    setContentNonOwned(mainView.get(), false);

#if defined JUCE_ANDROID || defined JUCE_IOS
    setFullScreen(true);
#else
    setBounds(50, 50, 800, 500);
#endif

    setVisible(true);

    // restore the last size and position from our settings file...
    restoreWindowStateFromString(Polytempo_StoredPreferences::getInstance()->getProps().getValue("mainWindow"));

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

void Polytempo_NetworkWindow::setBrightness(float brightness)
{
    mainView->setBrightness(brightness);
}

void Polytempo_NetworkWindow::setFullScreen(bool shouldBeFullScreen)
{
    if(fullScreen == shouldBeFullScreen) return;
    
    Desktop& desktop = Desktop::getInstance();

    if (shouldBeFullScreen && desktop.getKioskModeComponent() == nullptr)
        desktop.setKioskModeComponent(getTopLevelComponent());
    else
        desktop.setKioskModeComponent(nullptr);
    
    fullScreen = shouldBeFullScreen;

    Polytempo_NetworkApplication::getCommandManager()->commandStatusChanged(); // update menubar
}

bool Polytempo_NetworkWindow::isFullScreen()
{
    return fullScreen;
}

static void saveOkCancelCallback(int result, Polytempo_NetworkWindow* pParent)
{
    if (result)
    {
        Polytempo_NetworkApplication*const app = dynamic_cast<Polytempo_NetworkApplication *>(JUCEApplication::getInstance());
        app->saveScoreFile(true);
        if (app->scoreFileExists())
        {
            pParent->performSetContentID();
        }
    }
}

void Polytempo_NetworkWindow::setContentID(contentID newContentID)
{
    if (newContentID != currentContentID)
    {
        Polytempo_NetworkApplication*const app = dynamic_cast<Polytempo_NetworkApplication *>(JUCEApplication::getInstance());

        contentIdToBeSet = newContentID;
        if (newContentID == pageEditorViewID && !app->scoreFileExists())
        {
            Polytempo_OkCancelAlert::show("Save?",
                                          "You must first save the document before you can access the Page Editor",
                                          ModalCallbackFunction::create(saveOkCancelCallback, this));
        }
        else if (currentContentID == scoreEditorViewID && newContentID != scoreEditorViewID)
        {
            if (scoreEditorView->applyChanges())
                performSetContentID();
        }
        else
            performSetContentID();
    }
}

void Polytempo_NetworkWindow::performSetContentID()
{
    currentContentID = contentIdToBeSet;

    if (currentContentID == pageEditorViewID)
    {
        pageEditorView->refresh();
        setContentNonOwned(pageEditorView.get(), false);
    }
    else if (currentContentID == regionEditorViewID)
    {
        regionEditorView->refresh();
        setContentNonOwned(regionEditorView.get(), false);
    }
    else if (currentContentID == scoreEditorViewID)
    {
        scoreEditorView->refresh();
        setContentNonOwned(scoreEditorView.get(), false);
    }
    else
    {
        setContentNonOwned(mainView.get(), false);

        // apply all changes that should be visible in the main view
        Polytempo_ImageManager::getInstance()->deleteAll();
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
    if (currentContentID == mainViewID) return mainView.get();
    if (currentContentID == pageEditorViewID) return pageEditorView.get();
    if (currentContentID == regionEditorViewID) return regionEditorView.get();
    if (currentContentID == scoreEditorViewID) return scoreEditorView.get();

    return nullptr;
}

bool Polytempo_NetworkWindow::applyChanges()
{
    if (currentContentID == scoreEditorViewID)
        return scoreEditorView->applyChanges();

    return true;
}
