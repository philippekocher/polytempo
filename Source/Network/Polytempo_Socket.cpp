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
	socket = new DatagramSocket(true);
	port = port_;
	socket->bindToPort(0, Polytempo_NetworkSupervisor::getInstance()->getLocalAddress());
	hostName = new String(hostName_);
	ticks = ticks_;
}

Polytempo_Socket::~Polytempo_Socket()
{
	socket = nullptr;
	hostName = nullptr;
}

void Polytempo_Socket::renew()
{
	socket = new DatagramSocket(true);
	socket->bindToPort(0, Polytempo_NetworkSupervisor::getInstance()->getLocalAddress());
}

void Polytempo_Socket::write(const void *sourceBuffer, int numBytesToWrite)
{
	socket->write(*hostName, port, sourceBuffer, numBytesToWrite);
}

bool Polytempo_Socket::transmitsTicks() { return ticks; }

