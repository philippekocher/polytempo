#pragma once

#include "JuceHeader.h"

class Polytempo_NetworkInfoView : public Component
{
public:
    Polytempo_NetworkInfoView();
    ~Polytempo_NetworkInfoView() override;

    void paint(Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_NetworkInfoView)
};
