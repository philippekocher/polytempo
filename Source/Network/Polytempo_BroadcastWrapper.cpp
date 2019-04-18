/*
  ==============================================================================

    Polytempo_BroadcastWrapper.cpp
    Created: 31 Mar 2019 7:53:18pm
    Author:  christian.schweizer

  ==============================================================================
*/

#include "Polytempo_BroadcastWrapper.h"
#include "Polytempo_NetworkSupervisor.h"

Polytempo_BroadcastWrapper::Polytempo_BroadcastWrapper(int port)
{
	this->port = port;
	startTimer(2000);
}

Polytempo_BroadcastWrapper::~Polytempo_BroadcastWrapper()
{
	stopTimer();

	HashMap < Uuid, Polytempo_AsyncUnicastSender* >::Iterator it(unicastSenderList);
	while (it.next())
	{
		delete it.getValue();
	}

	unicastSenderList.clear();
}

void Polytempo_BroadcastWrapper::UpdatePeers(HashMap<Uuid, Polytempo_PeerInfo>* pPeers)
{
	HashMap < Uuid, Polytempo_PeerInfo >::Iterator it(*pPeers);
	while (it.next())
	{
		if(!unicastSenderList.contains(it.getKey()))
		{
			unicastSenderList.set(it.getKey(), new Polytempo_AsyncUnicastSender(it.getValue().ip, port));
		}
	}

	// check for peers to remove
	HashMap < Uuid, Polytempo_AsyncUnicastSender* >::Iterator it2(unicastSenderList);
	while (it2.next())
	{
		if (!pPeers->contains(it2.getKey()))
		{
			unicastSenderList.remove(it2.getKey());
		}
	}
}

void Polytempo_BroadcastWrapper::SendEvent(Polytempo_Event* pEvent) const
{
	HashMap < Uuid, Polytempo_AsyncUnicastSender* >::Iterator it(unicastSenderList);
	while (it.next())
	{
		it.getValue()->sendAsync(pEvent);
	}
}

void Polytempo_BroadcastWrapper::SendOsc(OSCMessage* pMsg) const
{
	HashMap < Uuid, Polytempo_AsyncUnicastSender* >::Iterator it(unicastSenderList);
	while (it.next())
	{
		it.getValue()->sendAsync(pMsg);
	}
}

void Polytempo_BroadcastWrapper::timerCallback()
{
	UpdatePeers(Polytempo_NetworkSupervisor::getInstance()->getPeers());
}
