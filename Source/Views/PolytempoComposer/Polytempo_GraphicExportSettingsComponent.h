#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
class Polytempo_GraphicExportView;
#include "../../Misc/Polytempo_Button.h"
#include "../../Misc/Polytempo_Textbox.h"


class Polytempo_GraphicExportSettingsComponent : public Component,
public Button::Listener,
public Label::Listener
{
public:
    Polytempo_GraphicExportSettingsComponent();
    ~Polytempo_GraphicExportSettingsComponent();
    
    void update(Polytempo_GraphicExportView*);
    
    void paint(Graphics&) override;
    void resized() override;

    void buttonClicked(Button*) override;
    void editorShown(Label*, TextEditor&) override;
    void labelTextChanged(Label*) override;

private:
    Polytempo_GraphicExportView *graphicExportView;
    Polytempo_Button* exportPagesButton;

    Polytempo_Textbox *timeFactorTextbox;
    String timeFactorString;
    
    Polytempo_Textbox *systemHeightTextbox;
    String systemHeightString;
    
    ToggleButton      *landscapeButton;



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_GraphicExportSettingsComponent)
};
