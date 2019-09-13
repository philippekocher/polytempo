#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Polytempo_GraphicExportPage.h"
#include "Polytempo_SequencesViewport.h"

class Polytempo_GraphicExportView : public Component
{
public:
    Polytempo_GraphicExportView();
    ~Polytempo_GraphicExportView();

    void paint(Graphics&) override;
    void resized() override;

private:
    Polytempo_GraphicExportViewport graphicExportViewport;
    Polytempo_SequencesViewport sequencesViewport;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_GraphicExportView)
};
