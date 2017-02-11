/*
  ==============================================================================

    Polytempo_Socket.cpp
    Created: 1 Feb 2017 6:31:37pm
    Author:  christian.schweizer

  ==============================================================================
*/

#include "Polytempo_Socket.h"
#include "Polytempo_NetworkSupervisor.h"

Polytempo_Socket::Polytempo_Socket(const String& hostName_, int port_, bool ticks_)
{
	oscSender = new OSCSender();
	port = port_;
	oscSender->connect(Polytempo_NetworkSupervisor::getInstance()->getLocalAddress(), 0);
	hostName = new String(hostName_);
	ticks = ticks_;
}

Polytempo_Socket::~Polytempo_Socket()
{
	oscSender = nullptr;
	hostName = nullptr;
}

void Polytempo_Socket::renew()
{
	oscSender->connect(Polytempo_NetworkSupervisor::getInstance()->getLocalAddress(), 0);
}

void Polytempo_Socket::write(OSCMessage oscMessage)
{
	oscSender->sendToIPAddress(*hostName, port, oscMessage);
}

bool Polytempo_Socket::transmitsTicks() { return ticks; }

