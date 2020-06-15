#pragma once

#include "Polytempo_ComposerWindow.h"
#include "JuceHeader.h"

class Polytempo_ComposerMenuBarModel : public MenuBarModel,
                                       public ApplicationCommandTarget

{
public:
    Polytempo_ComposerMenuBarModel(Polytempo_ComposerWindow*);
    ~Polytempo_ComposerMenuBarModel();
        
    StringArray getMenuBarNames();
    PopupMenu getMenuForIndex (int menuIndex, const String& menuName);
    void menuItemSelected (int menuID, int topLevelMenuIndex);

    ApplicationCommandTarget* getNextCommandTarget();
    void getAllCommands (Array <CommandID>& commands);
    void getCommandInfo (CommandID commandID, ApplicationCommandInfo& result);
	bool perform (const InvocationInfo& info);
    

private:
    Polytempo_ComposerWindow *window;
#if JUCE_MAC
    std::unique_ptr<PopupMenu> extraAppleMenuItems;
#endif

};