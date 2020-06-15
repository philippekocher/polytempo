#pragma once

#include "Polytempo_NetworkWindow.h"

class Polytempo_MenuBarModel : public MenuBarModel,
                               public ApplicationCommandTarget
{
public:
    Polytempo_MenuBarModel(Polytempo_NetworkWindow*);
    ~Polytempo_MenuBarModel();

    StringArray getMenuBarNames() override;
    PopupMenu getMenuForIndex(int menuIndex, const String& menuName) override;
    void menuItemSelected(int menuID, int topLevelMenuIndex) override;

    ApplicationCommandTarget* getNextCommandTarget() override;
    void getAllCommands(Array<CommandID>& commands) override;
    void getCommandInfo(CommandID commandID, ApplicationCommandInfo& result) override;
    bool perform(const InvocationInfo& info) override;

private:
    Polytempo_NetworkWindow* window;

#if JUCE_MAC
    std::unique_ptr<PopupMenu> extraAppleMenuItems;
#endif
};
