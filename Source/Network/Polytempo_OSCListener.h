#pragma once

#include "JuceHeader.h"
#include "../Scheduler/Polytempo_Event.h"

class Polytempo_OSCListener : OSCReceiver, OSCReceiver::Listener<OSCReceiver::RealtimeCallback>
{
public:
    Polytempo_OSCListener(int port);
    ~Polytempo_OSCListener() override;

private:
    void oscMessageReceived(const OSCMessage& message) override;
    Polytempo_Event* oscToEvent(const OSCMessage& message, String addressPattern) const;
    int m_Port;

    std::unique_ptr<OSCReceiver> oscReceiver;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_OSCListener)
};
