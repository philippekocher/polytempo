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
	messageToSend = nullptr;
}

void Polytempo_AsyncUnicastSender::sendAsync(Polytempo_Event* pEvent)
{
	const ScopedLock lock(csEvent);
	
	OSCMessage oscMessage = OSCMessage(OSCAddressPattern(pEvent->getOscAddressFromType()));
	for (var message : pEvent->getOscMessageFromProperties())
	{
		if (message.isInt())          oscMessage.addInt32(int(message));
		else if (message.isDouble())  oscMessage.addFloat32(float(message));
		else if (message.isString())  oscMessage.addString(message.toString());
	}

	messageToSend = new OSCMessage(oscMessage);

	triggerAsyncUpdate();
}

void Polytempo_AsyncUnicastSender::sendAsync(OSCMessage* pMsg)
{
	const ScopedLock lock(csEvent);

	messageToSend = new OSCMessage(*pMsg);

	triggerAsyncUpdate();
}

void Polytempo_AsyncUnicastSender::handleAsyncUpdate()
{
	const ScopedLock lock(csEvent);
	
	OSCMessage msg = OSCMessage(*messageToSend);
	socket->write(msg);
}
