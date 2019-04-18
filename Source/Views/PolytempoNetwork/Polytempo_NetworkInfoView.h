/*
  ==============================================================================

    Polytempo_NetworkInfoView.h
    Created: 5 Dec 2017 2:41:16pm
    Author:  christian.schweizer

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

//==============================================================================
/*
*/
class Polytempo_NetworkInfoView    : public Component
{
public:
    Polytempo_NetworkInfoView();
    ~Polytempo_NetworkInfoView();

    void paint (Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_NetworkInfoView)
};
