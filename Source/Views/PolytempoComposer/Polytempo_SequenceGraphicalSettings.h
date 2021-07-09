#pragma once

#include "../../Data/Polytempo_Sequence.h"
#include "../../Misc/Polytempo_Textbox.h"


class Polytempo_SequenceGraphicalSettings : public Component,
public Button::Listener,
public Label::Listener
{
public:
    Polytempo_SequenceGraphicalSettings(Polytempo_Sequence*);
    ~Polytempo_SequenceGraphicalSettings() override;
    
    void paint(Graphics&) override;
    void resized() override;
    
    /* listeners
     --------------------------------------- */
    void buttonClicked(Button* button) override;
    void labelTextChanged(Label* textbox) override;
    
    static void show(Polytempo_Sequence*);

private:
    Polytempo_Sequence* sequence;
    
    Polytempo_Textbox *numberOfStavesTextbox;
    Polytempo_Textbox *secondaryStaveOffsetTextbox;

    Polytempo_Textbox *staveOffsetTextbox;
    Polytempo_Textbox *numberOfLinesTextbox;
    Polytempo_Textbox *lineOffsetTextbox;
    
    ToggleButton      *showNameButton;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_SequenceGraphicalSettings)
};
