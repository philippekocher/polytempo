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

#include "Polytempo_NetworkMenuBarModel.h"
#include "../Polytempo_AboutWindow.h"
#include "Polytempo_NetworkApplication.h"
#include "../Polytempo_CommandIDs.h"
#include "../../Preferences/Polytempo_NetworkPreferencesPanel.h"
#include "../../Preferences/Polytempo_StoredPreferences.h"
#include "../../Scheduler/Polytempo_ScoreScheduler.h"
#include "../../Scheduler/Polytempo_EventDispatcher.h"
#include "../../Views/PolytempoNetwork/Polytempo_GraphicsAnnotationManager.h"


Polytempo_MenuBarModel::Polytempo_MenuBarModel(Polytempo_NetworkWindow *theWindow)
{
    window = theWindow;

    ApplicationCommandManager *commandManager = Polytempo_NetworkApplication::getCommandManager();

    commandManager->registerAllCommandsForTarget(this);
    commandManager->registerAllCommandsForTarget(JUCEApplication::getInstance());
    commandManager->setFirstCommandTarget(this);

    // tells our menu bar model that it should watch this command manager for
    // changes, and send change messages accordingly.
    setApplicationCommandManagerToWatch (commandManager);

#if JUCE_MAC
    // extra menu to be added to the mac osx app-menu
    extraAppleMenuItems = new PopupMenu();

    extraAppleMenuItems->addCommandItem(commandManager, Polytempo_CommandIDs::aboutWindow);
    extraAppleMenuItems->addSeparator();
    extraAppleMenuItems->addCommandItem(commandManager, Polytempo_CommandIDs::preferencesWindow);

    MenuBarModel::setMacMainMenu(this, extraAppleMenuItems);
#else
    window->setMenuBar(this);
#endif
}

Polytempo_MenuBarModel::~Polytempo_MenuBarModel()
{
#if JUCE_MAC
    MenuBarModel::setMacMainMenu(nullptr);
    extraAppleMenuItems = nullptr;
#endif
}

//------------------------------------------------------------------------------
#pragma mark -
#pragma mark menu

StringArray Polytempo_MenuBarModel::getMenuBarNames()
{
#if JUCE_MAC
    const char* const names[] = { "File", "Edit", "View", "Scheduler", /*"Window",*/ "Annotations", "Help", 0 };
#else
    const char* const names[] = { "PolytempoNetwork", "File", "Edit", "View", "Scheduler", "Annotations", /*"Window",*/ "Help", 0 };
#endif

    return StringArray (names);
}

/*
   Edit Menu: the keyboard shortcuts for Cut, Copy, Paste work without any commands in the edit menu
   Window Menu: there is no need for it
*/



PopupMenu Polytempo_MenuBarModel::getMenuForIndex(int, const String& menuName)
{
    ApplicationCommandManager *commandManager = Polytempo_NetworkApplication::getCommandManager();
    PopupMenu menu;

#if !JUCE_MAC
    if (menuName == "PolytempoNetwork")
    {
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::aboutWindow);
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::preferencesWindow);
        menu.addSeparator();
        menu.addCommandItem(commandManager, StandardApplicationCommandIDs::quit);
    }
#endif

    if (menuName == "File")
    {
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::newDocument);
        menu.addSeparator();
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::open);

        PopupMenu recentFilesMenu;
        Polytempo_StoredPreferences::getInstance()->recentFiles.createPopupMenuItems(recentFilesMenu, Polytempo_CommandIDs::openRecent, false, true);
        recentFilesMenu.addSeparator();
        recentFilesMenu.addCommandItem(commandManager, Polytempo_CommandIDs::clearOpenRecent);
        menu.addSubMenu("Open Recent", recentFilesMenu);

        menu.addSeparator();
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::save);
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::saveAs);
        menu.addSeparator();
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::close);
    }
    else if (menuName == "Edit")
    {
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::loadImage);
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::addSection);
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::addInstance);
        menu.addSeparator();
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::addRegion);
        menu.addSeparator();
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::deleteSelected);
    }
    else if (menuName == "View")
    {
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::showMainView);
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::showPageEditor);
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::showRegionEditor);
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::showScoreEditor);
        menu.addSeparator();
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::zoomIn);
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::zoomOut);
        menu.addSeparator();
#if ! JUCE_LINUX
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::fullScreen);
#endif
    }
    else if (menuName == "Scheduler")
    {
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::startStop);
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::returnToLoc);
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::returnToBeginning);

        menu.addSeparator();

        menu.addCommandItem(commandManager, Polytempo_CommandIDs::markerFwd);
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::markerBwd);
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::imageFwd);
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::imageBwd);

        menu.addSeparator();

        menu.addCommandItem(commandManager, Polytempo_CommandIDs::gotoTime);
    }
	else if (menuName == "Annotations")
	{
		menu.addCommandItem(commandManager, Polytempo_CommandIDs::annotationAccept);
		menu.addCommandItem(commandManager, Polytempo_CommandIDs::annotationCancel);
		menu.addCommandItem(commandManager, Polytempo_CommandIDs::annotationDelete);

		menu.addSeparator();

		menu.addCommandItem(commandManager, Polytempo_CommandIDs::annotationColor);
		menu.addCommandItem(commandManager, Polytempo_CommandIDs::annotationTextSize);
		menu.addCommandItem(commandManager, Polytempo_CommandIDs::annotationLayerSettings);
		menu.addCommandItem(commandManager, Polytempo_CommandIDs::annotationLayerEditMode);
	}
	else if (menuName == "Window")
    {

    }
    else if (menuName == "Help")
    {
#if ! JUCE_MAC
        menu.addCommandItem (commandManager, Polytempo_CommandIDs::aboutWindow);
#endif
        menu.addCommandItem (commandManager, Polytempo_CommandIDs::help);
        menu.addSeparator();
        menu.addCommandItem (commandManager, Polytempo_CommandIDs::visitWebsite);
    }

    return menu;
}

void Polytempo_MenuBarModel::menuItemSelected(int menuID, int)
{
    int fileIndex = menuID - Polytempo_CommandIDs::openRecent;
    if(fileIndex >= 0)
    {
        RecentlyOpenedFilesList recentFiles = Polytempo_StoredPreferences::getInstance()->recentFiles;
        Polytempo_NetworkApplication* const app = dynamic_cast<Polytempo_NetworkApplication*>(JUCEApplication::getInstance());
        app->openScoreFile(recentFiles.getFile(fileIndex));
        menuItemsChanged(); // refresh the recent files menu
    }
}

//------------------------------------------------------------------------------
#pragma mark -
#pragma mark commands

// The following methods implement the ApplicationCommandTarget interface, allowing
// this window to publish a set of actions it can perform, and which can be mapped
// onto menus, keypresses, etc.

ApplicationCommandTarget* Polytempo_MenuBarModel::getNextCommandTarget()
{
    // this will return the next parent component that is an ApplicationCommandTarget (in this
    // case, there probably isn't one, but it's best to use this method in your own apps).
    return findFirstTargetParentComponent();
}

void Polytempo_MenuBarModel::getAllCommands(Array <CommandID>& commands)
{
    // this returns the set of all commands that this target can perform..
    const CommandID ids[] = {
        Polytempo_CommandIDs::newDocument,
        Polytempo_CommandIDs::open,
        Polytempo_CommandIDs::clearOpenRecent,
        Polytempo_CommandIDs::save,
        Polytempo_CommandIDs::saveAs,
        Polytempo_CommandIDs::close,

        Polytempo_CommandIDs::loadImage,
        Polytempo_CommandIDs::addSection,
        Polytempo_CommandIDs::addInstance,
        Polytempo_CommandIDs::addRegion,
        Polytempo_CommandIDs::deleteSelected,

        Polytempo_CommandIDs::showMainView,
        Polytempo_CommandIDs::showPageEditor,
        Polytempo_CommandIDs::showRegionEditor,
        Polytempo_CommandIDs::showScoreEditor,
        Polytempo_CommandIDs::zoomIn,
        Polytempo_CommandIDs::zoomOut,
        Polytempo_CommandIDs::startStop,
        Polytempo_CommandIDs::returnToLoc,
        Polytempo_CommandIDs::returnToBeginning,
        Polytempo_CommandIDs::markerFwd,
        Polytempo_CommandIDs::markerBwd,
        Polytempo_CommandIDs::imageFwd,
        Polytempo_CommandIDs::imageBwd,
        //Polytempo_CommandIDs::gotoTime,

		Polytempo_CommandIDs::annotationAccept,
		Polytempo_CommandIDs::annotationCancel,
		Polytempo_CommandIDs::annotationDelete,
		Polytempo_CommandIDs::annotationColor,
		Polytempo_CommandIDs::annotationTextSize,
		Polytempo_CommandIDs::annotationLayerSettings,
		Polytempo_CommandIDs::annotationLayerEditMode,

        Polytempo_CommandIDs::aboutWindow,
        Polytempo_CommandIDs::preferencesWindow,
        Polytempo_CommandIDs::help,
        Polytempo_CommandIDs::visitWebsite
#if ! JUCE_LINUX
        , Polytempo_CommandIDs::fullScreen
#endif
    };

    commands.addArray (ids, numElementsInArray(ids));
}

// This method is used when something needs to find out the details about one of the commands
// that this object can perform..
void Polytempo_MenuBarModel::getCommandInfo(CommandID commandID, ApplicationCommandInfo& result)
{
    const String infoCategory ("Polytempo Network");

    Polytempo_NetworkApplication* const app = dynamic_cast<Polytempo_NetworkApplication*>(JUCEApplication::getInstance());

    switch(commandID)
    {
        /* polytempo network menu
         ----------------------------------*/
        case Polytempo_CommandIDs::preferencesWindow:
            result.setInfo("Preferences", "Show the preferences", infoCategory, 0);
            result.addDefaultKeypress(',', ModifierKeys::commandModifier);
            break;

        case Polytempo_CommandIDs::aboutWindow:
            result.setInfo ("About " + JUCEApplication::getInstance()->getApplicationName(), "About", infoCategory, 0);
            break;

        /* file menu
         ----------------------------------*/
        case Polytempo_CommandIDs::newDocument:
            result.setInfo("New", String(), infoCategory, 0);
            result.addDefaultKeypress('n', ModifierKeys::commandModifier);
            break;

        case Polytempo_CommandIDs::open:
            result.setInfo("Open...", "Opens a Polytempo score file", infoCategory, 0);
            result.addDefaultKeypress('o', ModifierKeys::commandModifier);
            break;

        case Polytempo_CommandIDs::clearOpenRecent:
            result.setInfo("Clear Menu", String(), infoCategory, 0);
            result.setActive(Polytempo_StoredPreferences::getInstance()->recentFiles.getNumFiles() > 0);
            break;

        case Polytempo_CommandIDs::save:
            result.setInfo("Save", "Saves the Polytempo score file", infoCategory, 0);
            result.addDefaultKeypress('s', ModifierKeys::commandModifier);
            result.setActive(app->getScore() != nullptr);
            break;

        case Polytempo_CommandIDs::saveAs:
            result.setInfo("Save As...", "Saves the Polytempo score file", infoCategory, 0);
            result.addDefaultKeypress('s', ModifierKeys::shiftModifier | ModifierKeys::commandModifier);
            result.setActive(app->getScore() != nullptr);
            break;

        case Polytempo_CommandIDs::close:
            result.setInfo("Close", String(), infoCategory, 0);
            result.addDefaultKeypress('w', ModifierKeys::commandModifier);
            break;

        /* edit menu
         ----------------------------------*/
        case Polytempo_CommandIDs::loadImage:
            result.setInfo("Load Image", "Load an image", infoCategory, 0);
            result.setActive(window->getContentID() == Polytempo_NetworkWindow::pageEditorViewID);
            break;

        case Polytempo_CommandIDs::addSection:
            result.setInfo("Add Section", "Add a section", infoCategory, 0);
            result.setActive(window->getContentID() == Polytempo_NetworkWindow::pageEditorViewID && ((Polytempo_PageEditorView*)window->getContentComponent())->hasSelectedImage());
            break;

        case Polytempo_CommandIDs::addInstance:
            result.setInfo("Add Instance", "Add an instance of the section", infoCategory, 0);
            result.setActive(window->getContentID() == Polytempo_NetworkWindow::pageEditorViewID && ((Polytempo_PageEditorView*)window->getContentComponent())->hasSelectedSection());
            break;

        case Polytempo_CommandIDs::addRegion:
            result.setInfo("Add Region", "Add a region", infoCategory, 0);
            result.setActive(window->getContentID() == Polytempo_NetworkWindow::regionEditorViewID);
            break;

        case Polytempo_CommandIDs::deleteSelected:
            result.setInfo("Delete selected", "Delete the selected item", infoCategory, 0);
            result.setActive(window->getContentID() == Polytempo_NetworkWindow::pageEditorViewID);
            break;


        /* view menu
         ----------------------------------*/
        case Polytempo_CommandIDs::showMainView:
            result.setInfo("Show Main View", String(), infoCategory, 0);
            result.addDefaultKeypress('1', ModifierKeys::ctrlModifier | ModifierKeys::commandModifier);
            result.addDefaultKeypress(KeyPress::numberPad1, ModifierKeys::ctrlModifier | ModifierKeys::commandModifier);
            break;

        case Polytempo_CommandIDs::showPageEditor:
            result.setInfo("Show Page Editor", String(), infoCategory, 0);
            result.addDefaultKeypress('2', ModifierKeys::ctrlModifier | ModifierKeys::commandModifier);
            result.addDefaultKeypress(KeyPress::numberPad2, ModifierKeys::ctrlModifier | ModifierKeys::commandModifier);
            break;

        case Polytempo_CommandIDs::showRegionEditor:
            result.setInfo("Show Region Editor", String(), infoCategory, 0);
            result.addDefaultKeypress('3', ModifierKeys::ctrlModifier | ModifierKeys::commandModifier);
            result.addDefaultKeypress(KeyPress::numberPad3, ModifierKeys::ctrlModifier | ModifierKeys::commandModifier);
            break;
            
        case Polytempo_CommandIDs::showScoreEditor:
            result.setInfo("Show Score Editor", String(), infoCategory, 0);
            result.addDefaultKeypress('4', ModifierKeys::ctrlModifier | ModifierKeys::commandModifier);
            result.addDefaultKeypress(KeyPress::numberPad4, ModifierKeys::ctrlModifier | ModifierKeys::commandModifier);
            break;
            
        case Polytempo_CommandIDs::zoomIn:
            result.setInfo("Zoom In", String(), infoCategory, 0);
            result.addDefaultKeypress(KeyPress::upKey, ModifierKeys::commandModifier);
            result.setActive(window->getContentID() == Polytempo_NetworkWindow::pageEditorViewID);
            break;

        case Polytempo_CommandIDs::zoomOut:
            result.setInfo("Zoom Out", String(), infoCategory, 0);
            result.addDefaultKeypress(KeyPress::downKey, ModifierKeys::commandModifier);
            result.setActive(window->getContentID() == Polytempo_NetworkWindow::pageEditorViewID);
            break;

#if ! JUCE_LINUX
        case Polytempo_CommandIDs::fullScreen:
            result.setInfo("Show full-screen", String(), infoCategory, 0);
            result.addDefaultKeypress('f', ModifierKeys::ctrlModifier | ModifierKeys::commandModifier);
            result.setTicked (window->isFullScreen());
            break;
#endif

        /* scheduler menu
         ----------------------------------*/
        case Polytempo_CommandIDs::startStop:
            result.setInfo("Start / Stop", String(), infoCategory, 0);
            result.addDefaultKeypress(' ', 0);
            result.setActive(window->getContentID() == Polytempo_NetworkWindow::mainViewID);
            break;

        case Polytempo_CommandIDs::returnToLoc:
            result.setInfo("Return to Locator", "Return to locator", infoCategory, 0);
            result.addDefaultKeypress('\r', 0);
            result.setActive(window->getContentID() == Polytempo_NetworkWindow::mainViewID);
            break;

        case Polytempo_CommandIDs::returnToBeginning:
            result.setInfo("Return to Beginning", String(), infoCategory, 0);
            result.addDefaultKeypress('\r', ModifierKeys::commandModifier);
            result.setActive(window->getContentID() == Polytempo_NetworkWindow::mainViewID);
            break;

        case Polytempo_CommandIDs::markerFwd:
            result.setInfo("Next Marker", String(), infoCategory, 0);
            result.addDefaultKeypress(KeyPress::rightKey, ModifierKeys::commandModifier);
            result.setActive(window->getContentID() == Polytempo_NetworkWindow::mainViewID);
            break;

        case Polytempo_CommandIDs::markerBwd:
            result.setInfo("Previous Marker", String(), infoCategory, 0);
            result.addDefaultKeypress(KeyPress::leftKey, ModifierKeys::commandModifier);
            result.setActive(window->getContentID() == Polytempo_NetworkWindow::mainViewID);
            break;

        case Polytempo_CommandIDs::imageFwd:
            result.setInfo("Next Image", String(), infoCategory, 0);
            result.addDefaultKeypress(KeyPress::rightKey, ModifierKeys::commandModifier | ModifierKeys::shiftModifier);
            result.setActive(window->getContentID() == Polytempo_NetworkWindow::mainViewID);
            break;

        case Polytempo_CommandIDs::imageBwd:
            result.setInfo("Previous Image", String(), infoCategory, 0);
            result.addDefaultKeypress(KeyPress::leftKey, ModifierKeys::commandModifier | ModifierKeys::shiftModifier);
            result.setActive(window->getContentID() == Polytempo_NetworkWindow::mainViewID);
            break;

        case Polytempo_CommandIDs::gotoTime:
            result.setInfo("Goto...", "Set a locator", infoCategory, 0);
            result.addDefaultKeypress('j', ModifierKeys::commandModifier);
            result.setActive(window->getContentID() == Polytempo_NetworkWindow::mainViewID);
            break;

		/* annotation menu
		 ------------------------------*/
		case Polytempo_CommandIDs::annotationAccept:
			result.setInfo("Accept", "Accecpt the current annotation", infoCategory, 0);
			result.addDefaultKeypress(KeyPress::returnKey, ModifierKeys::noModifiers);
			result.setActive(Polytempo_GraphicsAnnotationManager::getInstance()->isAnnotationPending());
			break;

		case Polytempo_CommandIDs::annotationCancel:
			result.setInfo("Cancel", "Cancel the current annotation", infoCategory, 0);
			result.addDefaultKeypress(KeyPress::escapeKey, ModifierKeys::noModifiers);
			result.setActive(Polytempo_GraphicsAnnotationManager::getInstance()->isAnnotationPending());
			break;

		case Polytempo_CommandIDs::annotationDelete:
			result.setInfo("Delete", "Delete the current annotation", infoCategory, 0);
			result.addDefaultKeypress(KeyPress::deleteKey, ModifierKeys::noModifiers);
			result.setActive(Polytempo_GraphicsAnnotationManager::getInstance()->isAnnotationPending() && Polytempo_GraphicsAnnotationManager::getInstance()->getAnchorFlag());
			break;

		case Polytempo_CommandIDs::annotationColor:
			result.setInfo("Color...", "Change the color of the current annotation", infoCategory, 0);
			result.addDefaultKeypress('c', ModifierKeys::noModifiers);
			result.setActive(Polytempo_GraphicsAnnotationManager::getInstance()->isAnnotationPending());
			break;

		case Polytempo_CommandIDs::annotationTextSize:
			result.setInfo("Text size...", "Change the text size of the current annotation", infoCategory, 0);
			result.addDefaultKeypress('t', ModifierKeys::noModifiers);
			result.setActive(Polytempo_GraphicsAnnotationManager::getInstance()->isAnnotationPending());
			break;

		case Polytempo_CommandIDs::annotationLayerSettings:
			result.setInfo("Layer settings...", "Annotation layer settings", infoCategory, 0);
			result.addDefaultKeypress('l', ModifierKeys::noModifiers);
			break;

		case Polytempo_CommandIDs::annotationLayerEditMode:
			result.setInfo("Edit mode", "Toggle edit mode", infoCategory, 0);
			result.addDefaultKeypress('e', ModifierKeys::noModifiers);
			result.setTicked(Polytempo_GraphicsAnnotationManager::getInstance()->getAnchorFlag());
			break;

        /* help menu
         ----------------------------------*/
        case Polytempo_CommandIDs::help:
            result.setInfo("PolytempoNetwork Help", String(), infoCategory, 0);
            result.addDefaultKeypress('?', ModifierKeys::commandModifier);
            break;

        case Polytempo_CommandIDs::visitWebsite:
            result.setInfo("Visit Polytempo Website", String(), infoCategory, 0);
            break;


        default:
            break;
    };
}

// this is the ApplicationCommandTarget method that is used to actually perform one of our commands..
bool Polytempo_MenuBarModel::perform(const InvocationInfo& info)
{
    Polytempo_NetworkApplication* const app = dynamic_cast<Polytempo_NetworkApplication*>(JUCEApplication::getInstance());
	Polytempo_GraphicsAnnotationLayer* pAnnotationLayer;

    switch(info.commandID)
    {
        /* polytempo network menu
         ----------------------------------*/

        case Polytempo_CommandIDs::preferencesWindow:
            Polytempo_NetworkPreferencesPanel::show();
            break;

        case Polytempo_CommandIDs::aboutWindow:
            Polytempo_AboutWindow::show();
            break;


        /* file menu
         ----------------------------------*/
        case Polytempo_CommandIDs::newDocument:
            app->newScore();
            break;

        case Polytempo_CommandIDs::open:
            app->openFileDialog();
            break;

        case Polytempo_CommandIDs::clearOpenRecent:
            Polytempo_StoredPreferences::getInstance()->recentFiles.clear();
            break;

        case Polytempo_CommandIDs::save:
            app->saveScoreFile(false);
            break;

        case Polytempo_CommandIDs::saveAs:
            app->saveScoreFile(true);
            break;

        case Polytempo_CommandIDs::close:
            app->systemRequestedQuit();
            break;


        /* edit menu
         ----------------------------------*/
        case Polytempo_CommandIDs::loadImage:
            ((Polytempo_PageEditorView*)window->getContentComponent())->loadImage();
            break;

        case Polytempo_CommandIDs::addSection:
            ((Polytempo_PageEditorView*)window->getContentComponent())->addSection();
            break;

        case Polytempo_CommandIDs::addInstance:
            ((Polytempo_PageEditorView*)window->getContentComponent())->addInstance();
            break;

        case Polytempo_CommandIDs::addRegion:
            ((Polytempo_RegionEditorView*)window->getContentComponent())->addRegion();
            break;

        case Polytempo_CommandIDs::deleteSelected:
            ((Polytempo_PageEditorView*)window->getContentComponent())->deleteSelected();
            break;


        /* view menu
         ----------------------------------*/
        case Polytempo_CommandIDs::showMainView:
            window->setContentID(Polytempo_NetworkWindow::mainViewID);
            break;

        case Polytempo_CommandIDs::showPageEditor:
            window->setContentID(Polytempo_NetworkWindow::pageEditorViewID);
            break;

        case Polytempo_CommandIDs::showRegionEditor:
            window->setContentID(Polytempo_NetworkWindow::regionEditorViewID);
            break;
            
        case Polytempo_CommandIDs::showScoreEditor:
            window->setContentID(Polytempo_NetworkWindow::scoreEditorViewID);
            break;
            
        case Polytempo_CommandIDs::zoomIn:
            Polytempo_StoredPreferences::getInstance()->getProps().setValue("zoom", Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("zoom") * 1.2);
            break;

        case Polytempo_CommandIDs::zoomOut:
            Polytempo_StoredPreferences::getInstance()->getProps().setValue("zoom", Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("zoom") / 1.2);
            break;

#if ! JUCE_LINUX
        case Polytempo_CommandIDs::fullScreen:
        {
            Desktop& desktop = Desktop::getInstance();

            if (desktop.getKioskModeComponent() == nullptr)
                desktop.setKioskModeComponent(window->getTopLevelComponent());
            else
                desktop.setKioskModeComponent(nullptr);
            break;
        }
#endif

        /* scheduler menu
         ----------------------------------*/
        case Polytempo_CommandIDs::startStop:
            Polytempo_ScoreScheduler::getInstance()->startStop();
            break;

        case Polytempo_CommandIDs::returnToLoc:
            Polytempo_ScoreScheduler::getInstance()->returnToLocator();
            break;

        case Polytempo_CommandIDs::returnToBeginning:
            Polytempo_ScoreScheduler::getInstance()->returnToBeginning();
            break;

        case Polytempo_CommandIDs::markerFwd:
            Polytempo_ScoreScheduler::getInstance()->skipToEvent(eventType_Marker);
            break;

        case Polytempo_CommandIDs::markerBwd:
            Polytempo_ScoreScheduler::getInstance()->skipToEvent(eventType_Marker, true);
            break;

        case Polytempo_CommandIDs::imageFwd:
            Polytempo_ScoreScheduler::getInstance()->skipToEvent(eventType_Image);
            break;

        case Polytempo_CommandIDs::imageBwd:
            Polytempo_ScoreScheduler::getInstance()->skipToEvent(eventType_Image, true);
            break;

        case Polytempo_CommandIDs::gotoTime:
            break;


		case Polytempo_CommandIDs::annotationAccept:
			pAnnotationLayer = Polytempo_GraphicsAnnotationManager::getInstance()->getCurrentPendingAnnotationLayer();
    		if(pAnnotationLayer != nullptr)
				pAnnotationLayer->handleEndEditAccept();
    		break;

		case Polytempo_CommandIDs::annotationCancel:
			pAnnotationLayer = Polytempo_GraphicsAnnotationManager::getInstance()->getCurrentPendingAnnotationLayer();
    		if(pAnnotationLayer != nullptr)
				pAnnotationLayer->handleEndEditCancel();
			break;

    	case Polytempo_CommandIDs::annotationDelete:
			pAnnotationLayer = Polytempo_GraphicsAnnotationManager::getInstance()->getCurrentPendingAnnotationLayer();
    		if(pAnnotationLayer != nullptr)
				pAnnotationLayer->handleDeleteSelected();
			break;

		case Polytempo_CommandIDs::annotationColor:
			pAnnotationLayer = Polytempo_GraphicsAnnotationManager::getInstance()->getCurrentPendingAnnotationLayer();
    		if(pAnnotationLayer != nullptr)
				pAnnotationLayer->hitBtnColor();
    		break;

		case Polytempo_CommandIDs::annotationTextSize:
			pAnnotationLayer = Polytempo_GraphicsAnnotationManager::getInstance()->getCurrentPendingAnnotationLayer();
			if (pAnnotationLayer != nullptr)
				pAnnotationLayer->hitBtnTextSize();
			break;

		case Polytempo_CommandIDs::annotationLayerSettings:
			Polytempo_GraphicsAnnotationManager::getInstance()->showSettingsDialog();
			break;

		case Polytempo_CommandIDs::annotationLayerEditMode:
			Polytempo_GraphicsAnnotationManager::getInstance()->setAnchorFlag(!Polytempo_GraphicsAnnotationManager::getInstance()->getAnchorFlag());
			break;

        /* help menu
         ----------------------------------*/
        case Polytempo_CommandIDs::help:
            Polytempo_AboutWindow::show();
            break;

        case Polytempo_CommandIDs::visitWebsite:
            URL("http://polytempo.zhdk.ch").launchInDefaultBrowser();
            break;

        default:
            return false;
    };

    return true;
}


