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
    std::unique_ptr<PopupMenu> extraAppleMenuItems;
#endif

};


#endif /* defined(__PolytempoPolytempo__Polytempo_MenuBarModel__) */
