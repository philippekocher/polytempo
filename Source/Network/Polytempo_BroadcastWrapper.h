/*
  ==============================================================================

    Polytempo_BroadcastWrapper.h
    Created: 31 Mar 2019 7:53:18pm
    Author:  christian.schweizer

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "../Scheduler/Polytempo_Event.h"
#include "Polytempo_AsyncUnicastSender.h"
#include "Polytempo_PeerInfo.h"

class Polytempo_BroadcastWrapper : Timer
{
public:
	Polytempo_BroadcastWrapper(int port);
	~Polytempo_BroadcastWrapper();

	void UpdatePeers(HashMap< Uuid, Polytempo_PeerInfo >* pPeers);
	void SendEvent(Polytempo_Event* pEvent) const;
	void SendOsc(OSCMessage* pMsg) const;

private:
	void timerCallback() override;

	HashMap< Uuid, Polytempo_AsyncUnicastSender* > unicastSenderList;
	int port;
};
