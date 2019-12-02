#ifndef __Polytempo_NetworkPreferencesPanel__
#define __Polytempo_NetworkPreferencesPanel__

#include "JuceHeader.h"


class Polytempo_NetworkPreferencesPanel : public PreferencesPanel
{
public:
    Polytempo_NetworkPreferencesPanel();
    ~Polytempo_NetworkPreferencesPanel();
    
    Component* createComponentForPage(const String& pageName);

    static void show();
};


#endif // __Polytempo_NetworkPreferencesPanel__
