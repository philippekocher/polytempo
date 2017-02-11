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

#include "Polytempo_MidiClick.h"
#include "../Preferences/Polytempo_StoredPreferences.h"


Polytempo_MidiClick::Polytempo_MidiClick()
{
    downbeatPitch = Polytempo_StoredPreferences::getInstance()->getProps().getIntValue("midiDownbeatPitch");
    beatPitch = Polytempo_StoredPreferences::getInstance()->getProps().getIntValue("midiBeatPitch");
    downbeatVelocity = Polytempo_StoredPreferences::getInstance()->getProps().getIntValue("midiDownbeatVelocity");
    beatVelocity = Polytempo_StoredPreferences::getInstance()->getProps().getIntValue("midiBeatVelocity");
    channel = Polytempo_StoredPreferences::getInstance()->getProps().getIntValue("midiChannel");
    
    juce::StringArray midiDevices = MidiOutput::getDevices();
    outputDeviceIndex = midiDevices.indexOf(Polytempo_StoredPreferences::getInstance()->getProps().getValue("midiOutputDevice"));
    if(outputDeviceIndex < 0) outputDeviceIndex = 0;
    midiOutput = MidiOutput::openDevice(outputDeviceIndex);
}

Polytempo_MidiClick::~Polytempo_MidiClick()
{
    midiOutput = nullptr;

    clearSingletonInstance();
}

juce_ImplementSingleton(Polytempo_MidiClick);

void Polytempo_MidiClick::eventNotification(Polytempo_Event *event)
{
    if(midiOutput == nullptr) return;
    
    if(event->getType() == eventType_Beat)
    {
        // take pitch and volume from the event's properties
        if(!event->getProperty("midiPitch").isVoid())
        {
            int pitch = event->getProperty("midiPitch");
            float velocity = event->getProperty("midiVelocity");
            
            midiOutput->sendMessageNow(MidiMessage::noteOn(1, pitch, velocity));
            midiOutput->sendMessageNow(MidiMessage::noteOff(1, pitch, 0.0f));
        }
#ifdef POLYTEMPO_NETWORK
        // or take default values if midi is turned on
        else if(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("midiClick"))
        {
            int pattern = event->getProperty(eventPropertyString_Pattern);
            int pitch;
            uint8 velocity;
            
            if(pattern == 0) return;
            
            if(int(event->getProperty(eventPropertyString_Cue)) != 0)
            {
                pitch = cuePitch;
                velocity = (int8)cueVelocity;
            }
            else
            {
                if(pattern < 20)
                {
                    pitch = downbeatPitch;
                    velocity = (int8)downbeatVelocity;
                }
                else
                {
                    pitch = beatPitch;
                    velocity = (int8)beatVelocity;
                }
            }

            midiOutput->sendMessageNow(MidiMessage::noteOn(channel, pitch, velocity));
            midiOutput->sendMessageNow(MidiMessage::noteOff(channel, pitch, 0.0f));
        }
#endif
    }
}


void Polytempo_MidiClick::setDownbeatPitch(int value) { downbeatPitch = value; }
void Polytempo_MidiClick::setDownbeatVelocity(int value)  { downbeatVelocity = value; }
void Polytempo_MidiClick::setBeatPitch(int value)  { beatPitch = value; }
void Polytempo_MidiClick::setBeatVelocity(int value)  { beatVelocity = value; }
void Polytempo_MidiClick::setChannel(int value)  { channel = value; }

void Polytempo_MidiClick::setOutputDeviceIndex(int value)
{
    outputDeviceIndex = value;
    midiOutput = MidiOutput::openDevice(outputDeviceIndex);
}
