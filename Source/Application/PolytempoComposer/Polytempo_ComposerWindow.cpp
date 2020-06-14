#include "Polytempo_ComposerWindow.h"
#include "Polytempo_ComposerMenuBarModel.h"
#include "Polytempo_ComposerApplication.h"
#include "../../Preferences/Polytempo_StoredPreferences.h"


Polytempo_ComposerWindow::Polytempo_ComposerWindow()
: DocumentWindow (String(),
                  Colours::lightgrey,
                  DocumentWindow::allButtons)
{
    setUsingNativeTitleBar(true);
    setResizable(true,true);
    setResizeLimits(500, 400, 5000, 5000);

    mainView.reset(new Polytempo_ComposerMainView());
    graphicExportView.reset(new Polytempo_GraphicExportView());
    
    setContentNonOwned(mainView.get(), false);
    
    setBounds(50, 50, 800, 500);
    setVisible(true);    

    // look and feel (set before the main view is restored)
    lookAndFeel.setUsingNativeAlertWindows(true);
    LookAndFeel::setDefaultLookAndFeel(&lookAndFeel);

    // restore the window's properties from the settings file
    restoreWindowStateFromString(Polytempo_StoredPreferences::getInstance()->getProps().getValue("mainWindow"));
    restoreWindowContentStateFromString(Polytempo_StoredPreferences::getInstance()->getProps().getValue("mainWindowContent"));
    
    openGLContext.attachTo(*getTopLevelComponent());
    openGLContext.setContinuousRepainting(true);

    // create and manage a MenuBarComponent
	menuBarModel.reset(new Polytempo_ComposerMenuBarModel(this));

#if !JUCE_MAC
    setMenuBar(menuBarModel.get());
#endif
    
    // use keypresses that arrive in this window to send out commands
    ApplicationCommandManager* commandManager = &Polytempo_ComposerApplication::getCommandManager();
    addKeyListener(commandManager->getKeyMappings());
}

Polytempo_ComposerWindow::~Polytempo_ComposerWindow()
{
	setMenuBar(nullptr);
    mainView = nullptr;
    graphicExportView = nullptr;

    openGLContext.detach();
}

void Polytempo_ComposerWindow::closeButtonPressed()
{
    // This is called when the user tries to close this window. Here, we'll just
    // ask the app to quit when this happens, but you can change this to do
    // whatever you need.
    JUCEApplication::getInstance()->systemRequestedQuit();
}

void Polytempo_ComposerWindow::setContentID(contentID newContentID)
{
    if (newContentID == mainViewID)
    {
        setContentNonOwned(mainView.get(), false);
    }
    else if (newContentID == graphicExportViewID)
    {
        setContentNonOwned(graphicExportView.get(), false);
    }
    getContentComponent()->resized();
}

int Polytempo_ComposerWindow::getContentID()
{
    if(getContentComponent() == mainView.get()) return mainViewID;
    if(getContentComponent() == graphicExportView.get()) return graphicExportViewID;

    return -1;
}


String Polytempo_ComposerWindow::getWindowContentStateAsString()
{
    return mainView->getComponentStateAsString();
}

void Polytempo_ComposerWindow::restoreWindowContentStateFromString(const String& string)
{
    mainView->restoreComponentStateFromString(string);
}
