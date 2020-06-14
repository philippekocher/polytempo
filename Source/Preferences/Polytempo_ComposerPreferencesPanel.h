#pragma once

#include "JuceHeader.h"

class Polytempo_ComposerPreferencesPanel : public PreferencesPanel
{
public:
    Polytempo_ComposerPreferencesPanel();
    ~Polytempo_ComposerPreferencesPanel();
    
    Component* createComponentForPage(const String& pageName);
    
    static void show();
};
