#pragma once

#include "JuceHeader.h"

class Polytempo_NetworkPreferencesPanel : public PreferencesPanel
{
public:
    Polytempo_NetworkPreferencesPanel();
    ~Polytempo_NetworkPreferencesPanel();
    
    Component* createComponentForPage(const String& pageName);

    static void show();
};