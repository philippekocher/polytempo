#pragma once

#include "../Scheduler/Polytempo_EventObserver.h"

class Polytempo_MidiClick : public Polytempo_EventObserver
{
public:
    Polytempo_MidiClick();
    ~Polytempo_MidiClick();
    
    juce_DeclareSingleton (Polytempo_MidiClick, false);
    
    void eventNotification(Polytempo_Event *event);
    
    void setDownbeatPitch(int value);
    void setDownbeatVelocity(int value);
    void setBeatPitch(int value);
    void setBeatVelocity(int value);
    void setCuePitch(int value);
    void setCueVelocity(int value);
    void setChannel(int value);
    void setOutputDeviceIndex(int value);

private:
    int downbeatPitch, beatPitch, cuePitch;
    int downbeatVelocity, beatVelocity, cueVelocity;
    int channel, outputDeviceIndex;

	std::unique_ptr<MidiOutput> midiOutput;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_MidiClick)
};