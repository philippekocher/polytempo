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

#include "Polytempo_ComposerMenuBarModel.h"
#include "Polytempo_ComposerApplication.h"
#include "../Polytempo_AboutWindow.h"
#include "../../Preferences/Polytempo_ComposerPreferencesPanel.h"
#include "../../Preferences/Polytempo_StoredPreferences.h"
#include "../../Data/Polytempo_Composition.h"
#include "../../Scheduler/Polytempo_ScoreScheduler.h"
#include "../Polytempo_CommandIDs.h"
#include "../../Views/PolytempoComposer/Polytempo_DialogWindows.h"


Polytempo_ComposerMenuBarModel::Polytempo_ComposerMenuBarModel(Polytempo_ComposerWindow* w)
{
    window = w;
    ApplicationCommandManager* commandManager = &Polytempo_ComposerApplication::getCommandManager();

    commandManager->registerAllCommandsForTarget(this);
    commandManager->setFirstCommandTarget(this);
    
    // tells our menu bar model that it should watch this command manager for
    // changes, and send change messages accordingly.
    setApplicationCommandManagerToWatch(commandManager);
    
#if JUCE_MAC
    // extra menu to be added to the mac osx app-menu
    extraAppleMenuItems.reset(new PopupMenu());
    
    extraAppleMenuItems->addCommandItem(commandManager, Polytempo_CommandIDs::aboutWindow);
    extraAppleMenuItems->addSeparator();
    extraAppleMenuItems->addCommandItem(commandManager, Polytempo_CommandIDs::preferencesWindow);
    
    MenuBarModel::setMacMainMenu (this, extraAppleMenuItems.get());
#endif
    
}

Polytempo_ComposerMenuBarModel::~Polytempo_ComposerMenuBarModel()
{
#if JUCE_MAC
    MenuBarModel::setMacMainMenu(nullptr);
    extraAppleMenuItems = nullptr;
#endif
}


//------------------------------------------------------------------------------
#pragma mark -
#pragma mark menu

StringArray Polytempo_ComposerMenuBarModel::getMenuBarNames()
{
#if JUCE_MAC
    const char* const names[] = { "File", "Edit", "View", "Scheduler", /*"Window",*/ "Help", 0 };
#else
    const char* const names[] = { "PolytempoComposer", "File", "Edit", "View", "Scheduler", /*"Window",*/ "Help", 0 };
#endif
    
    return StringArray (names);
}

PopupMenu Polytempo_ComposerMenuBarModel::getMenuForIndex (int /*menuIndex*/, const String& menuName)
{
    ApplicationCommandManager* commandManager = &Polytempo_ComposerApplication::getCommandManager();
    PopupMenu menu;
    
#if !JUCE_MAC
    if (menuName == "PolytempoComposer")
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
        menu.addCommandItem (commandManager, Polytempo_CommandIDs::open);
        PopupMenu recentFilesMenu;
        Polytempo_StoredPreferences::getInstance()->recentFiles.createPopupMenuItems(recentFilesMenu, Polytempo_CommandIDs::openRecent, false, true);
        recentFilesMenu.addSeparator();
        recentFilesMenu.addCommandItem(commandManager, Polytempo_CommandIDs::clearOpenRecent);
        menu.addSubMenu("Open Recent", recentFilesMenu);
        menu.addSeparator();
        menu.addCommandItem (commandManager, Polytempo_CommandIDs::save);
        menu.addCommandItem (commandManager, Polytempo_CommandIDs::exportSelected);
        menu.addCommandItem (commandManager, Polytempo_CommandIDs::exportAll);
        menu.addSeparator();
        menu.addCommandItem (commandManager, Polytempo_CommandIDs::close);
    }
    else if (menuName == "Edit")
    {
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::addSequence);
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::removeSequence);
        menu.addSeparator();
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::addBeatPattern);
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::insertBeatPattern);
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::removeBeatPattern);
        menu.addSeparator();
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::insertControlPoint);
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::removeControlPoint);
        menu.addSeparator();
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::adjustTime);
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::adjustPosition);
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::adjustTempo);
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::alignWithCursor);
    }
    else if (menuName == "View")
    {
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::showMainView);
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::showGraphicExportView);
        menu.addSeparator();
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::showTimeMap);
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::showTempoMap);
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::showPatternList);
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::showPointList);
        menu.addSeparator();
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::zoomInX);
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::zoomOutX);
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::zoomInY);
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::zoomOutY);
#if ! JUCE_LINUX
        menu.addSeparator();
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::fullScreen);
#endif
    }    
    else if (menuName == "Scheduler")
    {
        menu.addCommandItem (commandManager, Polytempo_CommandIDs::startStop);
        menu.addCommandItem (commandManager, Polytempo_CommandIDs::returnToLoc);
        menu.addCommandItem (commandManager, Polytempo_CommandIDs::returnToBeginning);
        menu.addSeparator();
        //menu.addCommandItem (commandManager, gotoCmd);
    }

    else if (menuName == "Help")
    {
#if ! JUCE_MAC
        menu.addCommandItem (commandManager, Polytempo_CommandIDs::aboutWindow);
#endif
        //menu.addCommandItem (commandManager, helpCmd);
        menu.addSeparator();
        menu.addCommandItem(commandManager, Polytempo_CommandIDs::visitWebsite);
    }
    
    return menu;
}

void Polytempo_ComposerMenuBarModel::menuItemSelected (int menuID, int /*topLevelMenuIndex*/)
{
    int fileIndex = menuID - Polytempo_CommandIDs::openRecent;
    if(fileIndex >= 0)
    {
        RecentlyOpenedFilesList recentFiles = Polytempo_StoredPreferences::getInstance()->recentFiles;
        Polytempo_Composition::getInstance()->openFile(recentFiles.getFile(fileIndex));
        menuItemsChanged(); // refresh the recent files menu
    }
}

//------------------------------------------------------------------------------
#pragma mark -
#pragma mark commands

// The following methods implement the ApplicationCommandTarget interface,
// allowing to publish a set of actions it can perform, and which can be
// mapped onto menus, keypresses, etc.

ApplicationCommandTarget* Polytempo_ComposerMenuBarModel::getNextCommandTarget()
{
    // this will return the next parent component that is an ApplicationCommandTarget (in this
    // case, there probably isn't one, but it's best to use this method in your own apps).
    return findFirstTargetParentComponent();
}

void Polytempo_ComposerMenuBarModel::getAllCommands (Array <CommandID>& commands)
{
    // this returns the set of all commands that this target can perform..
    const CommandID ids[] = {
        Polytempo_CommandIDs::newDocument,
        Polytempo_CommandIDs::open,
        Polytempo_CommandIDs::clearOpenRecent,
        Polytempo_CommandIDs::close,
        Polytempo_CommandIDs::save,
        Polytempo_CommandIDs::exportSelected,
        Polytempo_CommandIDs::exportAll,

        Polytempo_CommandIDs::showMainView,
        Polytempo_CommandIDs::showGraphicExportView,

        Polytempo_CommandIDs::showTimeMap,
        Polytempo_CommandIDs::showTempoMap,
        Polytempo_CommandIDs::showPatternList,
        Polytempo_CommandIDs::showPointList,
        Polytempo_CommandIDs::zoomInX, Polytempo_CommandIDs::zoomOutX,
        Polytempo_CommandIDs::zoomInY, Polytempo_CommandIDs::zoomOutY,
        
        Polytempo_CommandIDs::startStop,
        Polytempo_CommandIDs::returnToLoc,
        Polytempo_CommandIDs::returnToBeginning,
        
        Polytempo_CommandIDs::addSequence,
        Polytempo_CommandIDs::removeSequence,
        Polytempo_CommandIDs::addBeatPattern,
        Polytempo_CommandIDs::insertBeatPattern,
        Polytempo_CommandIDs::removeBeatPattern,
        Polytempo_CommandIDs::insertControlPoint,
        Polytempo_CommandIDs::adjustTime,
        Polytempo_CommandIDs::adjustPosition,
        Polytempo_CommandIDs::adjustTempo,
        Polytempo_CommandIDs::alignWithCursor,
        Polytempo_CommandIDs::removeControlPoint,
        
        Polytempo_CommandIDs::aboutWindow,
        Polytempo_CommandIDs::preferencesWindow,
        //Polytempo_CommandIDs::help,
        Polytempo_CommandIDs::visitWebsite,

#if ! JUCE_LINUX
        Polytempo_CommandIDs::fullScreen
#endif
    };
    
    commands.addArray (ids, numElementsInArray(ids));
}

// This method is used when something needs to find out the details about one of the commands
// that this object can perform..
void Polytempo_ComposerMenuBarModel::getCommandInfo(CommandID commandID, ApplicationCommandInfo& result)
{
    const String infoCategory("Polytempo Composer");
    Polytempo_Composition *composition = Polytempo_Composition::getInstance();
    
    switch (commandID)
    {
        /* polytempo network menu
         ----------------------------------*/

        case Polytempo_CommandIDs::preferencesWindow:
            result.setInfo ("Preferences", "Show the preferences", infoCategory, 0);
            result.addDefaultKeypress (',', ModifierKeys::commandModifier);
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
            result.setInfo("Open...", String(), infoCategory, 0);
            result.addDefaultKeypress('o', ModifierKeys::commandModifier);
            break;
            
        case Polytempo_CommandIDs::clearOpenRecent:
            result.setInfo("Clear Menu", String(), infoCategory, 0);
            result.setActive(Polytempo_StoredPreferences::getInstance()->recentFiles.getNumFiles() > 0);
            break;

        case Polytempo_CommandIDs::save:
            result.setInfo("Save", String(), infoCategory, 0);
            result.addDefaultKeypress('s', ModifierKeys::commandModifier);
            break;

//        case Polytempo_CommandIDs::saveAs:

        case Polytempo_CommandIDs::exportSelected:
            result.setInfo("Export Selected Sequence...", String(), infoCategory, 0);
            result.addDefaultKeypress('e', ModifierKeys::commandModifier);
            break;

        case Polytempo_CommandIDs::exportAll:
            result.setInfo("Export All Sequences...", String(), infoCategory, 0);
            result.addDefaultKeypress('e', ModifierKeys::shiftModifier | ModifierKeys::commandModifier);
            break;

        case Polytempo_CommandIDs::close:
            result.setInfo("Close", String(), infoCategory, 0);
            result.addDefaultKeypress('w', ModifierKeys::commandModifier);
            break;
            
        /* edit menu
         ----------------------------------*/

        case Polytempo_CommandIDs::addSequence:
            result.setInfo ("Add Sequence", String(), infoCategory, 0);
            break;
            
        case Polytempo_CommandIDs::removeSequence:
            result.setInfo("Remove Selected Sequence", String(), infoCategory, 0);
            result.setActive(composition->getSelectedSequenceIndex() >= 0);
            break;

        case Polytempo_CommandIDs::addBeatPattern:
            result.setInfo("Add Beat Pattern", String(), infoCategory, 0);
            result.addDefaultKeypress('b', ModifierKeys::commandModifier);
            break;
            
        case Polytempo_CommandIDs::insertBeatPattern:
            result.setInfo("Insert Beat Pattern", String(), infoCategory, 0);
            result.addDefaultKeypress('b', ModifierKeys::commandModifier | ModifierKeys::altModifier);
            result.setActive(composition->getSelectedSequence() != nullptr && composition->getSelectedSequence()->getSelectedBeatPattern() >= 0);
            break;
            
        case Polytempo_CommandIDs::removeBeatPattern:
            result.setInfo ("Remove Selected Beat Pattern", String(), infoCategory, 0);
            result.setActive(composition->getSelectedSequence() != nullptr && composition->getSelectedSequence()->getSelectedBeatPattern() >= 0);
            break;
            
        case Polytempo_CommandIDs::insertControlPoint:
            result.setInfo ("Insert Control Point", String(), infoCategory, 0);
            result.setActive(composition->getSelectedSequence() != nullptr && composition->getSelectedSequence()->getControlPoints()->size() > 0);
            break;
            
        case Polytempo_CommandIDs::adjustTime:
            result.setInfo ("Adjust Time", String(), infoCategory, 0);
            result.setActive(composition->getSelectedSequence()->allowAdjustTime(composition->getSelectedControlPointIndex()));
            break;
            
        case Polytempo_CommandIDs::adjustPosition:
            result.setInfo ("Adjust Position", String(), infoCategory, 0);
            result.setActive(composition->isSelectedControlPointRemovable());
            break;
            
        case Polytempo_CommandIDs::adjustTempo:
            result.setInfo ("Adjust Tempo", String(), infoCategory, 0);
            result.setActive(composition->getSelectedControlPointIndex() > 0);
            break;
            
        case Polytempo_CommandIDs::alignWithCursor:
            result.setInfo ("Align With Cursor", String(), infoCategory, 0);
            result.setActive(composition->getSelectedControlPointIndex() > 0 &&
                             !Polytempo_ScoreScheduler::getInstance()->isRunning());
            break;
            
        case Polytempo_CommandIDs::removeControlPoint:
            result.setInfo("Remove Selected Control Point", String(), infoCategory, 0);
            result.setActive(composition->isSelectedControlPointRemovable());
            break;

            
        /* view menu
         ----------------------------------*/

        case Polytempo_CommandIDs::showMainView:
            result.setInfo ("Show Main View", String(), infoCategory, 0);
            result.addDefaultKeypress('1', ModifierKeys::ctrlModifier |ModifierKeys::commandModifier);
            result.addDefaultKeypress(KeyPress::numberPad1, ModifierKeys::ctrlModifier | ModifierKeys::commandModifier);
            break;
            
        case Polytempo_CommandIDs::showGraphicExportView:
            result.setInfo ("Show Graphic Export View", String(), infoCategory, 0);
            result.addDefaultKeypress('2', ModifierKeys::ctrlModifier |ModifierKeys::commandModifier);
            result.addDefaultKeypress(KeyPress::numberPad1, ModifierKeys::ctrlModifier | ModifierKeys::commandModifier);
            break;
            
        case Polytempo_CommandIDs::showTimeMap:
            result.setInfo ("Show/Hide Time Map", String(), infoCategory, 0);
            result.addDefaultKeypress('1', ModifierKeys::commandModifier);
            result.addDefaultKeypress(KeyPress::numberPad1, ModifierKeys::commandModifier);
            break;
            
        case Polytempo_CommandIDs::showTempoMap:
            result.setInfo ("Show/Hide Tempo Map", String(), infoCategory, 0);
            result.addDefaultKeypress('2', ModifierKeys::commandModifier);
            result.addDefaultKeypress(KeyPress::numberPad2, ModifierKeys::commandModifier);
            break;
            
        case Polytempo_CommandIDs::showPatternList:
            result.setInfo ("Show/Hide Pattern List", String(), infoCategory, 0);
            result.addDefaultKeypress('3', ModifierKeys::commandModifier);
            result.addDefaultKeypress(KeyPress::numberPad3, ModifierKeys::commandModifier);
            break;
            
        case Polytempo_CommandIDs::showPointList:
            result.setInfo ("Show/Hide Point List", String(), infoCategory, 0);
            result.addDefaultKeypress('4', ModifierKeys::commandModifier);
            result.addDefaultKeypress(KeyPress::numberPad4, ModifierKeys::commandModifier);
            break;
            
        case Polytempo_CommandIDs::zoomInX:
            result.setInfo ("Horizontal Zoom In", String(), infoCategory, 0);
            result.addDefaultKeypress (KeyPress::rightKey, ModifierKeys::commandModifier);
            break;
            
        case Polytempo_CommandIDs::zoomOutX:
            result.setInfo ("Horizontal Zoom Out", String(), infoCategory, 0);
            result.addDefaultKeypress (KeyPress::leftKey, ModifierKeys::commandModifier);
            break;
            
        case Polytempo_CommandIDs::zoomInY:
            result.setInfo ("Vertical Zoom In", String(), infoCategory, 0);
            result.addDefaultKeypress (KeyPress::upKey, ModifierKeys::commandModifier);
            break;
            
        case Polytempo_CommandIDs::zoomOutY:
            result.setInfo ("Vertical Zoom Out", String(), infoCategory, 0);
            result.addDefaultKeypress (KeyPress::downKey, ModifierKeys::commandModifier);
            break;
            
#if ! JUCE_LINUX
        case Polytempo_CommandIDs::fullScreen:
            result.setInfo ("Enter full-screen", String(), infoCategory, 0);
            result.addDefaultKeypress ('f', ModifierKeys::ctrlModifier | ModifierKeys::commandModifier);
            result.setTicked (Desktop::getInstance().getKioskModeComponent() != 0);
            break;
#endif

        /* scheduler menu
         ----------------------------------*/
            
        case Polytempo_CommandIDs::startStop:
            result.setInfo ("Start / Stop", "Start the playback", infoCategory, 0);
            result.addDefaultKeypress (' ', 0);
            break;
            
        case Polytempo_CommandIDs::returnToLoc:
            result.setInfo ("Return to Locator", "Return to locator", infoCategory, 0);
            result.addDefaultKeypress ('\r', 0);
            break;
            
        case Polytempo_CommandIDs::returnToBeginning:
            result.setInfo ("Return to beginning", "Return to beginning", infoCategory, 0);
            result.addDefaultKeypress ('\r', ModifierKeys::commandModifier);
            break;

            /*
             case gotoCmd:
             result.setInfo ("Goto...", "Set a locator", infoCategory, 0);
             result.addDefaultKeypress ('j', ModifierKeys::commandModifier);
             break;
             */
            
            
        /* help menu
         ----------------------------------*/
        case Polytempo_CommandIDs::help:
            result.setInfo ("Polytempo Help", String(), infoCategory, 0);
            result.addDefaultKeypress ('?', ModifierKeys::commandModifier);
            break;
            
        case Polytempo_CommandIDs::visitWebsite:
            result.setInfo("Visit Polytempo Website", String(), infoCategory, 0);
            break;

        default:
            break;
    };
}

// this is the ApplicationCommandTarget method that is used to actually perform one of our commands..
bool Polytempo_ComposerMenuBarModel::perform (const InvocationInfo& info)
{
    float zoomX, zoomY;
    Polytempo_Composition *composition = Polytempo_Composition::getInstance();
    
    switch (info.commandID)
    {
        /* polytempo composer menu
         ----------------------------------*/
            
        case Polytempo_CommandIDs::preferencesWindow:
            Polytempo_ComposerPreferencesPanel::show();
            break;
            
        case Polytempo_CommandIDs::aboutWindow:
            Polytempo_AboutWindow::show();
            break;

            
        /* file menu
         ----------------------------------*/
            
        case Polytempo_CommandIDs::newDocument:
            composition->newComposition();
            break;
            
        case Polytempo_CommandIDs::open:
            composition->openFile();
            break;
            
        case Polytempo_CommandIDs::clearOpenRecent:
            Polytempo_StoredPreferences::getInstance()->recentFiles.clear();
            break;

        case Polytempo_CommandIDs::save:
            composition->saveToFile();
            break;
            
//        case Polytempo_CommandIDs::saveAs:
            
        case Polytempo_CommandIDs::exportSelected:
            Polytempo_Composition::getInstance()->exportSelectedSequence();
            break;
            
        case Polytempo_CommandIDs::exportAll:
            Polytempo_Composition::getInstance()->exportAllSequences();
            break;
            
        case Polytempo_CommandIDs::close:
            JUCEApplication::getInstance()->systemRequestedQuit();
            break;
            
        /* edit menu
         ----------------------------------*/
            
        case Polytempo_CommandIDs::addSequence:
            composition->addSequence();
            break;
            
        case Polytempo_CommandIDs::removeSequence:
            composition->removeSequence(composition->getSelectedSequenceIndex());
            break;
            
        case Polytempo_CommandIDs::addBeatPattern:
            composition->getSelectedSequence()->addBeatPattern();
            break;
            
        case Polytempo_CommandIDs::insertBeatPattern:
            composition->getSelectedSequence()->insertBeatPattern();
            break;
            
        case Polytempo_CommandIDs::removeBeatPattern:
            composition->getSelectedSequence()->removeSelectedBeatPattern();
            break;
            
        case Polytempo_CommandIDs::insertControlPoint:
            Polytempo_DialogWindows::InsertControlPoint().show();
            break;

        case Polytempo_CommandIDs::adjustTime:
            composition->getSelectedSequence()->adjustTime(composition->getSelectedControlPointIndex());
            break;
            
        case Polytempo_CommandIDs::adjustPosition:
            composition->getSelectedSequence()->adjustPosition(composition->getSelectedControlPointIndex());
            break;
            
        case Polytempo_CommandIDs::adjustTempo:
            composition->getSelectedSequence()->adjustTempo(composition->getSelectedControlPointIndex());
            break;
            
        case Polytempo_CommandIDs::alignWithCursor:
            composition->getSelectedSequence()->setControlPointPosition(composition->getSelectedControlPointIndex(), Polytempo_ScoreScheduler::getInstance()->getScoreTime() * 0.001f, -1);
            break;
            
        case Polytempo_CommandIDs::removeControlPoint:
            composition->getSelectedSequence()->removeControlPoint(composition->getSelectedControlPointIndex());
            composition->setSelectedControlPointIndex(-1);
            break;
            
            
        /* view menu
         ----------------------------------*/
        case Polytempo_CommandIDs::showMainView:
            window->setContentID(Polytempo_ComposerWindow::mainViewID);
            break;
        case Polytempo_CommandIDs::showGraphicExportView:
            window->setContentID(Polytempo_ComposerWindow::graphicExportViewID);
            break;
        case Polytempo_CommandIDs::showTimeMap:
            Polytempo_ComposerApplication::getMainView().setLeftComponent(Polytempo_ComposerMainView::componentType_TimeMap);
            break;
        case Polytempo_CommandIDs::showTempoMap:
            Polytempo_ComposerApplication::getMainView().setLeftComponent(Polytempo_ComposerMainView::componentType_TempoMap);
            break;
        case Polytempo_CommandIDs::showPatternList:
            Polytempo_ComposerApplication::getMainView().setRightComponent(Polytempo_ComposerMainView::componentType_PatternList);
            break;
        case Polytempo_CommandIDs::showPointList:
            Polytempo_ComposerApplication::getMainView().setRightComponent(Polytempo_ComposerMainView::componentType_PointList);
            break;
            
#if ! JUCE_LINUX
        case Polytempo_CommandIDs::fullScreen:
        {
            Desktop& desktop = Desktop::getInstance();
            Polytempo_ComposerApplication* const app = dynamic_cast<Polytempo_ComposerApplication*>(JUCEApplication::getInstance());

            if (desktop.getKioskModeComponent() == nullptr)
                desktop.setKioskModeComponent(app->getDocumentWindow().getTopLevelComponent());
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
            
            
        case Polytempo_CommandIDs::zoomInX:
            zoomX = float(Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("zoomX") * 1.2);
            zoomX = zoomX > 100 ? 100 : zoomX;
            Polytempo_StoredPreferences::getInstance()->getProps().setValue("zoomX", zoomX);
            break;
            
        case Polytempo_CommandIDs::zoomOutX:
            zoomX = float(Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("zoomX") / 1.2);
            zoomX = zoomX < 0.2f ? 0.2f : zoomX;
            Polytempo_StoredPreferences::getInstance()->getProps().setValue("zoomX", zoomX);
            break;
            
        case Polytempo_CommandIDs::zoomInY:
            if(Polytempo_ComposerApplication::getMainView().getLeftComponent() == Polytempo_ComposerMainView::componentType_TimeMap)
            {
                Polytempo_StoredPreferences::getInstance()->getProps().setValue("timeMapZoomY", Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("timeMapZoomY") * 1.2);
            }
            else
            {
                zoomY = float(Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("tempoMapZoomY") * 1.1);
                zoomY = zoomY > 15000 ? 15000 : zoomY;
                Polytempo_StoredPreferences::getInstance()->getProps().setValue("tempoMapZoomY", zoomY);
            }
            break;
            
        case Polytempo_CommandIDs::zoomOutY:
            if(Polytempo_ComposerApplication::getMainView().getLeftComponent() == Polytempo_ComposerMainView::componentType_TimeMap)
            {
                Polytempo_StoredPreferences::getInstance()->getProps().setValue("timeMapZoomY", Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("timeMapZoomY") / 1.2);
            }
            else
            {
                zoomY = float(Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("tempoMapZoomY") / 1.1);
                zoomY = zoomY < 500 ? 500 : zoomY;
                Polytempo_StoredPreferences::getInstance()->getProps().setValue("tempoMapZoomY", zoomY);
            }
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


