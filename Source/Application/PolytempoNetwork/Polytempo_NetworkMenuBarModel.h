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

#ifndef __PolytempoPolytempo__Polytempo_MenuBarModel__
#define __PolytempoPolytempo__Polytempo_MenuBarModel__

#include "Polytempo_NetworkWindow.h"


class Polytempo_MenuBarModel : public MenuBarModel,
                               public ApplicationCommandTarget

{
public:
    Polytempo_MenuBarModel(Polytempo_NetworkWindow*);
    ~Polytempo_MenuBarModel();
        
    StringArray getMenuBarNames();
    PopupMenu getMenuForIndex (int menuIndex, const String& menuName);
    void menuItemSelected (int menuID, int topLevelMenuIndex);

    ApplicationCommandTarget* getNextCommandTarget();
    void getAllCommands (Array <CommandID>& commands);
    void getCommandInfo (CommandID commandID, ApplicationCommandInfo& result);
	bool perform (const InvocationInfo& info);
    
    void showAboutWindow();
    
private:
    
    Polytempo_NetworkWindow *window;
    
#if JUCE_MAC
    ScopedPointer<PopupMenu> extraAppleMenuItems;
#endif

};


#endif /* defined(__PolytempoPolytempo__Polytempo_MenuBarModel__) */
