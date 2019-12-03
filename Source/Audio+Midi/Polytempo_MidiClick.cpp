#include "Polytempo_MidiClick.h"
#include "../Preferences/Polytempo_StoredPreferences.h"

Polytempo_MidiClick::Polytempo_MidiClick()
{
    downbeatPitch = Polytempo_StoredPreferences::getInstance()->getProps().getIntValue("midiDownbeatPitch");
    beatPitch = Polytempo_StoredPreferences::getInstance()->getProps().getIntValue("midiBeatPitch");
    cuePitch = Polytempo_StoredPreferences::getInstance()->getProps().getIntValue("midiCuePitch");
    downbeatVelocity = Polytempo_StoredPreferences::getInstance()->getProps().getIntValue("midiDownbeatVelocity");
    beatVelocity = Polytempo_StoredPreferences::getInstance()->getProps().getIntValue("midiBeatVelocity");
    cueVelocity = Polytempo_StoredPreferences::getInstance()->getProps().getIntValue("midiCueVelocity");
    channel = Polytempo_StoredPreferences::getInstance()->getProps().getIntValue("midiChannel");
    
    StringArray midiDevices = MidiOutput::getDevices();
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
            int pattern;
            int pitch;
            uint8 velocity;
            
            if(event->hasProperty(eventPropertyString_Pattern))
                pattern = event->getProperty(eventPropertyString_Pattern);
            else
                pattern = eventPropertyDefault_Pattern;
            
            if(pattern < 3) return;

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
void Polytempo_MidiClick::setCuePitch(int value)  { cuePitch = value; }
void Polytempo_MidiClick::setCueVelocity(int value)  { cueVelocity = value; }
void Polytempo_MidiClick::setChannel(int value)  { channel = value; }

void Polytempo_MidiClick::setOutputDeviceIndex(int value)
{
    outputDeviceIndex = value;
    midiOutput = MidiOutput::openDevice(outputDeviceIndex);
}
