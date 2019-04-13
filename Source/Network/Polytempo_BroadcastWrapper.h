/*
  ==============================================================================

    Polytempo_BroadcastWrapper.h
    Created: 31 Mar 2019 7:53:18pm
    Author:  chris

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "../Scheduler/Polytempo_Event.h"
#include "Polytempo_AsyncUnicastSender.h"

class Polytempo_BroadcastWrapper : Timer
{
public:
	Polytempo_BroadcastWrapper();
	~Polytempo_BroadcastWrapper();

	void UpdatePeers(HashMap< String, String >* pPeers);
	void SendEvent(Polytempo_Event* pEvent) const;
	void SendOsc(OSCMessage* pMsg) const;

private:
	void timerCallback() override;

	HashMap< String, Polytempo_AsyncUnicastSender* > unicastSenderList;
};
