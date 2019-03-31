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
}

void Polytempo_AsyncUnicastSender::sendAsync(Polytempo_Event* pEvent)
{
	const ScopedLock lock(csEvent);
	eventToSend = *pEvent;
	triggerAsyncUpdate();
}

void Polytempo_AsyncUnicastSender::handleAsyncUpdate()
{
	Polytempo_Event e;
	{
		const ScopedLock lock(csEvent);
		e = eventToSend;
	}

	OSCMessage oscMessage = OSCMessage(OSCAddressPattern(e.getOscAddressFromType()));
	for (var message : e.getOscMessageFromProperties())
	{
		if (message.isInt())          oscMessage.addInt32(int(message));
		else if (message.isDouble())  oscMessage.addFloat32(float(message));
		else if (message.isString())  oscMessage.addString(message.toString());
	}

	socket->write(oscMessage);
}
