#pragma once

#include "JuceHeader.h"

class Polytempo_DefaultPreferences : public PropertySet

{
public:

    Polytempo_DefaultPreferences();

    ~Polytempo_DefaultPreferences();

    juce_DeclareSingleton (Polytempo_DefaultPreferences, false);
};
