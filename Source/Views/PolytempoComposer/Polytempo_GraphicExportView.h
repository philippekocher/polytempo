#pragma once

#include "JuceHeader.h"
#include "Polytempo_GraphicExportPage.h"
#include "Polytempo_GraphicExportSettingsComponent.h"
#include "Polytempo_SequencesViewport.h"

class Polytempo_GraphicExportView : public Component
{
public:
    Polytempo_GraphicExportView();
    ~Polytempo_GraphicExportView() override;

    void paint(Graphics&) override;
    void resized() override;
    
    void setTimeFactor(float);
    int  getSystemHeight();
    void setSystemHeight(int);
    void setLandscape(bool);

    void update();
    void addPage();
    
    void exportPages();

private:
    Polytempo_GraphicExportViewport graphicExportViewport;
    Polytempo_GraphicExportSettingsComponent graphicExportSettingsComponent;
    Polytempo_SequencesViewport sequencesViewport;

    OwnedArray <Polytempo_GraphicExportPage> pages;
    float timeFactor;
    int systemHeight;
    bool landscape = false;
    int numberOfSequences = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_GraphicExportView)
};
