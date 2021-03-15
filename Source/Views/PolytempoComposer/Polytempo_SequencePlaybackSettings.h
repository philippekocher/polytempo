#pragma once

#include "../../Data/Polytempo_Sequence.h"
#include "../../Misc/Polytempo_Textbox.h"
#include "../../Misc/Polytempo_Button.h"


class Polytempo_SequencePlaybackSettings : public Component,
                                           public Button::Listener,
                                           public Label::Listener
{
public:
    Polytempo_SequencePlaybackSettings(Polytempo_Sequence*);
    ~Polytempo_SequencePlaybackSettings();
    
    void paint(Graphics&) override;
    void resized() override;
    
    /* listeners
     --------------------------------------- */
    void buttonClicked(Button* button) override;
    void labelTextChanged(Label* textbox) override;
    
    static void show(Polytempo_Sequence*);

private:
    Polytempo_Sequence* sequence;
    
    ToggleButton      *audioClick;
    Polytempo_Textbox *audioDownbeatPitch;
    Polytempo_Textbox *audioDownbeatVolume;
    Polytempo_Textbox *audioBeatPitch;
    Polytempo_Textbox *audioBeatVolume;
    Polytempo_Textbox *audioCuePitch;
    Polytempo_Textbox *audioCueVolume;
    Polytempo_Textbox *audioChannel;

    ToggleButton      *midiClick;
    Polytempo_Textbox *midiDownbeatPitch;
    Polytempo_Textbox *midiDownbeatVelocity;
    Polytempo_Textbox *midiBeatPitch;
    Polytempo_Textbox *midiBeatVelocity;
    Polytempo_Textbox *midiCuePitch;
    Polytempo_Textbox *midiCueVelocity;
    Polytempo_Textbox *midiChannel;
    
    ToggleButton *sendOsc;
    Polytempo_Textbox *oscDownbeatMessage;
    Polytempo_Button  *oscDownbeatDefaultButton;
    Polytempo_Textbox *oscBeatMessage;
    Polytempo_Button  *oscBeatDefaultButton;
    Polytempo_Textbox *oscCueMessage;
    Polytempo_Button  *oscCueDefaultButton;
    Polytempo_Textbox *oscReceiver;
    Polytempo_Button  *oscLocalhostButton;
    Polytempo_Textbox *oscPort;
    Polytempo_Button  *oscPortDefaultButton;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_SequencePlaybackSettings)
};
