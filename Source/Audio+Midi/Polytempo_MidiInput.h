#pragma once

#include "Polytempo_AudioDeviceManager.h"

class Polytempo_MidiInput : public MidiInputCallback
{
public:
    Polytempo_MidiInput()
    {
    }
    ~Polytempo_MidiInput()
    {
    }
    
    void handleIncomingMidiMessage(MidiInput*, const MidiMessage& message) override
    {
		DBG(message.getDescription());
    }
};