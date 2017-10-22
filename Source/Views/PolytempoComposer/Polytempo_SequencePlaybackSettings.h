/* ==============================================================================
 
 This file is part of the POLYTEMPO software package.
 Copyright (c) 2016 - Zurich University of the Arts,
 ICST Institute for Computer Music and Sound Technology
 http://www.icst.net
 
 Author: Philippe Kocher
 
 POLYTEMPO is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 POLYTEMPO is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this software. If not, see <http://www.gnu.org/licenses/>.
 
 ============================================================================== */

#ifndef __Polytempo_SequencePlaybackSettings__
#define __Polytempo_SequencePlaybackSettings__

#include "../../Data/Polytempo_Sequence.h"
#include "../../Misc/Polytempo_Textbox.h"
#include "../../Misc/Polytempo_Button.h"


class Polytempo_SequencePlaybackSettings : public Component,
                                           public ButtonListener,
                                           public LabelListener
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
    
    void updateOscReceiver();

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
    Polytempo_Button  *oscBroadcastButton;
    Polytempo_Textbox *oscPort;
    Polytempo_Button  *oscPortDefaultButton;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_SequencePlaybackSettings)
};


#endif // __Polytempo_SequencePlaybackSettings__
