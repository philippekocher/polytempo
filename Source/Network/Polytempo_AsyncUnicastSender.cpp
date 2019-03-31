/*
  ==============================================================================

    Polytempo_UnicastSender.cpp
    Created: 31 Mar 2019 7:51:32pm
    Author:  chris

  ==============================================================================
*/

#include "Polytempo_AsyncUnicastSender.h"

Polytempo_AsyncUnicastSender::Polytempo_AsyncUnicastSender(String ip)
{
	socket = new Polytempo_Socket(ip, 47522);
}

Polytempo_AsyncUnicastSender::~Polytempo_AsyncUnicastSender()
{
	socket = nullptr;
	eventToSend = nullptr;
}

void Polytempo_AsyncUnicastSender::sendAsync(Polytempo_Event* pEvent)
{
	const ScopedLock lock(csEvent);
	eventToSend = new Polytempo_Event(*pEvent);
	triggerAsyncUpdate();
}

void Polytempo_AsyncUnicastSender::handleAsyncUpdate()
{
	ScopedPointer < Polytempo_Event > e;
	{
		const ScopedLock lock(csEvent);
		e = new Polytempo_Event(*eventToSend);
	}

	OSCMessage oscMessage = OSCMessage(OSCAddressPattern(e->getOscAddressFromType()));
	for (var message : e->getOscMessageFromProperties())
	{
		if (message.isInt())          oscMessage.addInt32(int(message));
		else if (message.isDouble())  oscMessage.addFloat32(float(message));
		else if (message.isString())  oscMessage.addString(message.toString());
	}

	socket->write(oscMessage);
}
