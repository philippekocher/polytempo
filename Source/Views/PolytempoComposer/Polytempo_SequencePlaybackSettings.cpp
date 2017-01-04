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

#include "Polytempo_SequencePlaybackSettings.h"
#include "../../Application/PolytempoComposer/Polytempo_ComposerApplication.h"
#include "../../Data/Polytempo_Sequence.h"
#include "../../Scheduler/Polytempo_EventDispatcher.h"


Polytempo_SequencePlaybackSettings::Polytempo_SequencePlaybackSettings(Polytempo_Sequence* theSequence)
:sequence(theSequence)
{
    /* audio
     -------------------------------------------------- */
    
    addAndMakeVisible(audioClick = new ToggleButton(String::empty));
    audioClick->setButtonText("Play Audio Click");
    audioClick->setToggleState(sequence->playAudioClick, dontSendNotification);
    audioClick->addListener(this);
    
    
    // audio downbeat
    
    addAndMakeVisible(audioDownbeatPitch = new Polytempo_Textbox("Downbeat Pitch"));
    audioDownbeatPitch->setText(String(sequence->audioDownbeatPitch), dontSendNotification);
    audioDownbeatPitch->setEnabled(sequence->playAudioClick);
    audioDownbeatPitch->setInputRestrictions(0, "0123456789.",0,127);
    audioDownbeatPitch->addListener(this);
    
    addAndMakeVisible(audioDownbeatVolume = new Polytempo_Textbox("Downbeat Volume"));
    audioDownbeatVolume->setText(String(sequence->audioDownbeatVolume), dontSendNotification);
    audioDownbeatVolume->setEnabled(sequence->playAudioClick);
    audioDownbeatVolume->setInputRestrictions(0, "0123456789.",0,1);
    audioDownbeatVolume->addListener(this);
    
    
    // audio beat
    
    addAndMakeVisible(audioBeatPitch = new Polytempo_Textbox("Beat Pitch"));
    audioBeatPitch->setText(String(sequence->audioBeatPitch), dontSendNotification);
    audioBeatPitch->setEnabled(sequence->playAudioClick);
    audioBeatPitch->setInputRestrictions(0, "0123456789.",0,127);
    audioBeatPitch->addListener(this);
    
    addAndMakeVisible(audioBeatVolume = new Polytempo_Textbox("Beat Volume"));
    audioBeatVolume->setText(String(sequence->audioBeatVolume), dontSendNotification);
    audioBeatVolume->setEnabled(sequence->playAudioClick);
    audioBeatVolume->setInputRestrictions(0, "0123456789.",0,1);
    audioBeatVolume->addListener(this);
    
   
    // audio cue in

    addAndMakeVisible(audioCuePitch = new Polytempo_Textbox("Cue Pitch"));
    audioCuePitch->setText(String(sequence->audioCuePitch), dontSendNotification);
    audioCuePitch->setEnabled(sequence->playAudioClick);
    audioCuePitch->setInputRestrictions(0, "0123456789.",0,127);
    audioCuePitch->addListener(this);
    
    addAndMakeVisible(audioCueVolume = new Polytempo_Textbox("Cue Volume"));
    audioCueVolume->setText(String(sequence->audioCueVolume), dontSendNotification);
    audioCueVolume->setEnabled(sequence->playAudioClick);
    audioCueVolume->setInputRestrictions(0, "0123456789.",0,1);
    audioCueVolume->addListener(this);
    
    
    // output channel
 
    addAndMakeVisible(audioChannel = new Polytempo_Textbox("Hardware Channel"));
    audioChannel->setText(String(sequence->audioChannel), dontSendNotification);
    audioChannel->setEnabled(sequence->playAudioClick);
    audioChannel->setInputRestrictions(0, "0123456789",1,9999);
    audioChannel->addListener(this);

    
    /* midi
     -------------------------------------------------- */
    
    addAndMakeVisible(midiClick = new ToggleButton(String::empty));
    midiClick->setButtonText("Play Midi Click");
    midiClick->setToggleState(sequence->playMidiClick, dontSendNotification);
    midiClick->addListener(this);
    
    
    // midi downbeat
    
    addAndMakeVisible(midiDownbeatPitch = new Polytempo_Textbox("Downbeat Pitch"));
    midiDownbeatPitch->setText(String(sequence->midiDownbeatPitch), dontSendNotification);
    midiDownbeatPitch->setEnabled(sequence->playMidiClick);
    midiDownbeatPitch->setInputRestrictions(0, "0123456789.",0,127);
    midiDownbeatPitch->addListener(this);
    
    addAndMakeVisible(midiDownbeatVelocity = new Polytempo_Textbox("Downbeat Velocity"));
    midiDownbeatVelocity->setText(String(sequence->midiDownbeatVelocity), dontSendNotification);
    midiDownbeatVelocity->setEnabled(sequence->playMidiClick);
    midiDownbeatVelocity->setInputRestrictions(0, "0123456789.",0,127);
    midiDownbeatVelocity->addListener(this);
    
    
    // midi beat
    
    addAndMakeVisible(midiBeatPitch = new Polytempo_Textbox("Beat Pitch"));
    midiBeatPitch->setText(String(sequence->midiBeatPitch), dontSendNotification);
    midiBeatPitch->setEnabled(sequence->playMidiClick);
    midiBeatPitch->setInputRestrictions(0, "0123456789.",0,127);
    midiBeatPitch->addListener(this);
    
    addAndMakeVisible(midiBeatVelocity = new Polytempo_Textbox("Beat Velocity"));
    midiBeatVelocity->setText(String(sequence->midiBeatVelocity), dontSendNotification);
    midiBeatVelocity->setEnabled(sequence->playMidiClick);
    midiBeatVelocity->setInputRestrictions(0, "0123456789.",0,127);
    midiBeatVelocity->addListener(this);
    
    
    // midi cue in
    
    addAndMakeVisible(midiCuePitch = new Polytempo_Textbox("Cue Pitch"));
    midiCuePitch->setText(String(sequence->midiCuePitch), dontSendNotification);
    midiCuePitch->setEnabled(sequence->playMidiClick);
    midiCuePitch->setInputRestrictions(0, "0123456789.",0,127);
    midiCuePitch->addListener(this);
    
    addAndMakeVisible(midiCueVelocity = new Polytempo_Textbox("Cue Velocity"));
    midiCueVelocity->setText(String(sequence->midiCueVelocity), dontSendNotification);
    midiCueVelocity->setEnabled(sequence->playMidiClick);
    midiCueVelocity->setInputRestrictions(0, "0123456789.",0,127);
    midiCueVelocity->addListener(this);
    

    // output channel
    
    addAndMakeVisible(midiChannel = new Polytempo_Textbox("Midi Channel"));
    midiChannel->setText(String(sequence->midiChannel), dontSendNotification);
    midiChannel->setEnabled(sequence->playMidiClick);
    midiChannel->setInputRestrictions(0, "0123456789",1,16);
    midiChannel->addListener(this);
   
    
    /* osc
     -------------------------------------------------- */
    
    addAndMakeVisible(sendOsc = new ToggleButton(String::empty));
    sendOsc->setButtonText("Send Osc");
    sendOsc->setToggleState(sequence->sendOsc, dontSendNotification);
    sendOsc->addListener(this);
    
    addAndMakeVisible(oscDownbeatMessage = new Polytempo_Textbox("Downbeat Message"));
    oscDownbeatMessage->setText(sequence->oscDownbeatMessage, dontSendNotification);
    oscDownbeatMessage->setEnabled(sequence->sendOsc);
    oscDownbeatMessage->addListener(this);
    addAndMakeVisible(oscDownbeatDefaultButton = new Polytempo_Button("Default"));
    oscDownbeatDefaultButton->setEnabled(sequence->sendOsc);
    oscDownbeatDefaultButton->addListener(this);

    addAndMakeVisible(oscBeatMessage = new Polytempo_Textbox("Beat Message"));
    oscBeatMessage->setText(sequence->oscBeatMessage, dontSendNotification);
    oscBeatMessage->setEnabled(sequence->sendOsc);
    oscBeatMessage->addListener(this);
    addAndMakeVisible(oscBeatDefaultButton = new Polytempo_Button("Default"));
    oscBeatDefaultButton->setEnabled(sequence->sendOsc);
    oscBeatDefaultButton->addListener(this);

    addAndMakeVisible(oscCueMessage = new Polytempo_Textbox("Cue Message"));
    oscCueMessage->setText(sequence->oscCueMessage, dontSendNotification);
    oscCueMessage->setEnabled(sequence->sendOsc);
    oscCueMessage->addListener(this);
    addAndMakeVisible(oscCueDefaultButton = new Polytempo_Button("Default"));
    oscCueDefaultButton->setEnabled(sequence->sendOsc);
    oscCueDefaultButton->addListener(this);

    addAndMakeVisible(oscReceiver = new Polytempo_Textbox("Receiver (IP-Address or Node Name)"));
    oscReceiver->setText(sequence->oscReceiver, dontSendNotification);
    oscReceiver->setEnabled(sequence->sendOsc);
    oscReceiver->addListener(this);

    addAndMakeVisible(oscBroadcastButton = new Polytempo_Button("Broadcast"));
    oscBroadcastButton->setEnabled(sequence->sendOsc);
    oscBroadcastButton->addListener(this);

    addAndMakeVisible(oscPort = new Polytempo_Textbox("Port"));
    oscPort->setText(sequence->oscPort, dontSendNotification);
    oscPort->setEnabled(sequence->sendOsc);
    oscPort->setInputRestrictions(0, "0123456789");
    oscPort->addListener(this);
    
    addAndMakeVisible(oscPortDefaultButton = new Polytempo_Button("Default"));
    oscPortDefaultButton->setEnabled(sequence->sendOsc);
    oscPortDefaultButton->addListener(this);
}

Polytempo_SequencePlaybackSettings::~Polytempo_SequencePlaybackSettings()
{
    deleteAllChildren();
}

void Polytempo_SequencePlaybackSettings::resized()
{
    audioClick->setBounds              (20, 10, getWidth() * 0.5 - 40, 24);
    
    audioDownbeatPitch->setBounds      (20, 60, getWidth() * 0.25 - 30, 20);
    audioDownbeatVolume->setBounds     (getWidth() * 0.25 + 5, 60, getWidth() * 0.25 - 30, 20);
    audioBeatPitch->setBounds          (20, 100, getWidth() * 0.25 - 30, 20);
    audioBeatVolume->setBounds         (getWidth() * 0.25 + 5, 100, getWidth() * 0.25 - 30, 20);
    audioCuePitch->setBounds           (20, 140, getWidth() * 0.25 - 30, 20);
    audioCueVolume->setBounds          (getWidth() * 0.25 + 5, 140, getWidth() * 0.25 - 30, 20);
    audioChannel->setBounds            (20, 200, getWidth() * 0.25 - 30, 20);
    
    midiClick->setBounds               (getWidth() * 0.5 + 20,  10, getWidth() * 0.5 - 40, 24);
    
    midiDownbeatPitch->setBounds       (getWidth() * 0.5 + 20, 60, getWidth() * 0.25 - 30, 20);
    midiDownbeatVelocity->setBounds    (getWidth() * 0.75 + 5, 60, getWidth() * 0.25 - 30, 20);
    midiBeatPitch->setBounds           (getWidth() * 0.5 + 20, 100, getWidth() * 0.25 - 30, 20);
    midiBeatVelocity->setBounds        (getWidth() * 0.75 + 5, 100, getWidth() * 0.25 - 30, 20);
    midiCuePitch->setBounds            (getWidth() * 0.5 + 20, 140, getWidth() * 0.25 - 30, 20);
    midiCueVelocity->setBounds         (getWidth() * 0.75 + 5, 140, getWidth() * 0.25 - 30, 20);
    midiChannel->setBounds             (getWidth() * 0.5 + 20, 200, getWidth() * 0.25 - 30, 20);
    
    sendOsc->setBounds                  (20, 300, getWidth() - 40, 24);
    
    oscDownbeatMessage->setBounds       (20, 350, getWidth() - 120, 20);
    oscDownbeatDefaultButton->setBounds (getWidth() - 90, 350, 70, 20);
    oscBeatMessage->setBounds           (20, 390, getWidth() - 120, 20);
    oscBeatDefaultButton->setBounds     (getWidth() - 90, 390, 70, 20);
    oscCueMessage->setBounds            (20, 430, getWidth() - 120, 20);
    oscCueDefaultButton->setBounds      (getWidth() - 90, 430, 70, 20);
    oscReceiver->setBounds              (20, 500, getWidth() - 120, 20);
    oscBroadcastButton->setBounds       (getWidth() - 90, 500, 70, 20);
    oscPort->setBounds                  (20, 540, getWidth() - 120, 20);
    oscPortDefaultButton->setBounds     (getWidth() - 90, 540, 70, 20);
}


void Polytempo_SequencePlaybackSettings::paint (Graphics& g)
{
    g.fillAll (Colours::white);
}

void Polytempo_SequencePlaybackSettings::buttonClicked(Button* button)
{
    bool buttonState = button->getToggleState();
    
    if(button == audioClick)
    {
        sequence->playAudioClick = buttonState;
        
        audioDownbeatPitch->setEnabled(buttonState);
        audioDownbeatVolume->setEnabled(buttonState);
        audioBeatPitch->setEnabled(buttonState);
        audioBeatVolume->setEnabled(buttonState);
        audioCuePitch->setEnabled(buttonState);
        audioCueVolume->setEnabled(buttonState);
        audioChannel->setEnabled(buttonState);
    }
    else if(button == midiClick)
    {
        sequence->playMidiClick = buttonState;
        
        midiDownbeatPitch->setEnabled(buttonState);
        midiDownbeatVelocity->setEnabled(buttonState);
        midiBeatPitch->setEnabled(buttonState);
        midiBeatVelocity->setEnabled(buttonState);
        midiCuePitch->setEnabled(buttonState);
        midiCueVelocity->setEnabled(buttonState);
        midiChannel->setEnabled(buttonState);
    }
    else if(button == sendOsc)
    {
        sequence->sendOsc = buttonState;
        
        oscDownbeatMessage->setEnabled(buttonState);
        oscDownbeatDefaultButton->setEnabled(buttonState);
        oscBeatMessage->setEnabled(buttonState);
        oscBeatDefaultButton->setEnabled(buttonState);
        oscCueMessage->setEnabled(buttonState);
        oscCueDefaultButton->setEnabled(buttonState);
        oscReceiver->setEnabled(buttonState);
        oscBroadcastButton->setEnabled(buttonState);
        oscPort->setEnabled(buttonState);
        oscPortDefaultButton->setEnabled(buttonState);
    }
    else if(button == oscDownbeatDefaultButton)
    {
        oscDownbeatMessage->setText("/beat pattern #pattern duration #duration", dontSendNotification);
    }
    else if(button == oscBeatDefaultButton)
    {
        oscBeatMessage->setText("/beat pattern #pattern duration #duration", dontSendNotification);
    }
    else if(button == oscCueDefaultButton)
    {
        oscCueMessage->setText("/beat pattern #pattern duration #duration cue 1", dontSendNotification);
    }
    else if(button == oscBroadcastButton)
    {
        oscReceiver->setText("#broadcast", dontSendNotification);
    }
    else if(button == oscPortDefaultButton)
    {
        oscPort->setText("47522", dontSendNotification);
    }
    
    
}

void Polytempo_SequencePlaybackSettings::labelTextChanged(Label* textbox)
{
    String labelText = textbox->getText();
    
    if(textbox == audioDownbeatPitch)       sequence->audioDownbeatPitch = labelText.getFloatValue();
    else if(textbox == audioDownbeatVolume) sequence->audioDownbeatVolume = labelText.getFloatValue();
    else if(textbox == audioBeatPitch)      sequence->audioBeatPitch = labelText.getFloatValue();
    else if(textbox == audioBeatVolume)     sequence->audioBeatVolume = labelText.getFloatValue();
    else if(textbox == audioCuePitch)       sequence->audioCuePitch = labelText.getFloatValue();
    else if(textbox == audioCueVolume)      sequence->audioCueVolume = labelText.getFloatValue();
    else if(textbox == audioChannel)        sequence->audioChannel = labelText.getIntValue();
    
    else if(textbox == midiDownbeatPitch)   sequence->midiDownbeatPitch = labelText.getFloatValue();
    else if(textbox == midiDownbeatVelocity)sequence->midiDownbeatVelocity = labelText.getFloatValue();
    else if(textbox == midiBeatPitch)       sequence->midiBeatPitch = labelText.getFloatValue();
    else if(textbox == midiBeatVelocity)    sequence->midiBeatVelocity = labelText.getFloatValue();
    else if(textbox == midiCuePitch)        sequence->midiCuePitch = labelText.getFloatValue();
    else if(textbox == midiCueVelocity)     sequence->midiCueVelocity = labelText.getFloatValue();
    
    else if(textbox == oscDownbeatMessage)  sequence->oscDownbeatMessage = labelText;
    else if(textbox == oscBeatMessage)      sequence->oscBeatMessage = labelText;
    else if(textbox == oscCueMessage)       sequence->oscCueMessage = labelText;
    
    else if(textbox == oscReceiver)
    {
        sequence->oscReceiver = labelText;
        updateOscReceiver();
    }
    else if(textbox == oscPort)
    {
        sequence->oscPort = labelText;
        updateOscReceiver();
    }
}

void Polytempo_SequencePlaybackSettings::updateOscReceiver()
{
    Polytempo_Event *event = new Polytempo_Event(eventType_AddSender);
    event->setProperty("senderID", "sequence"+String(sequence->sequenceIndex));
    event->setProperty("ip", sequence->oscReceiver);
    event->setProperty("port", sequence->oscPort);
    
    Polytempo_EventDispatcher::getInstance()->broadcastEvent(event);
}


void Polytempo_SequencePlaybackSettings::show(Polytempo_Sequence* sequence)
{
    Polytempo_ComposerApplication* const app = dynamic_cast<Polytempo_ComposerApplication*>(JUCEApplication::getInstance());
    Polytempo_SequencePlaybackSettings settings(sequence);
    
    settings.setBounds(0,0,500,590);
 
    DialogWindow::LaunchOptions options;
    options.dialogTitle = "Playback Settings ("+sequence->getName()+")";
    options.content.setNonOwned(&settings);
    options.componentToCentreAround = &app->getMainView();
    options.useNativeTitleBar = true;
    options.escapeKeyTriggersCloseButton = true;
    options.resizable = false;
    
    options.runModal();
}
