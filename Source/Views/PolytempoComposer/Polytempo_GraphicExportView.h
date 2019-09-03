#pragma once

#include "../JuceLibraryCode/JuceHeader.h"


class Polytempo_GraphicExportView : public Component
{
public:
    Polytempo_GraphicExportView();
    ~Polytempo_GraphicExportView();

    void paint(Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_GraphicExportView)
};
