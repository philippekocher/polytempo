/*
  ==============================================================================

    Polytempo_UnicastSender.h
    Created: 31 Mar 2019 7:51:32pm
    Author:  christian.schweizer

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "../Scheduler/Polytempo_Event.h"
#include "Polytempo_Socket.h"

class Polytempo_AsyncUnicastSender : AsyncUpdater
{
public:
	Polytempo_AsyncUnicastSender(String ip, int port);
	~Polytempo_AsyncUnicastSender();

	void sendAsync(Polytempo_Event* pEvent);
	void sendAsync(OSCMessage* pMsg);
	void updateIp(String ip, int port);

private:
	void handleAsyncUpdate() override;

	CriticalSection csEvent;
	ScopedPointer < OSCMessage > messageToSend;
	ScopedPointer < Polytempo_Socket > socket;
	String currentIp;
	int currentPort;
};
