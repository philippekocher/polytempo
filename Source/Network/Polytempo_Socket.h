/*
  ==============================================================================

    Polytempo_Socket.h
    Created: 1 Feb 2017 6:31:37pm
    Author:  christian.schweizer

  ==============================================================================
*/

#ifndef POLYTEMPO_SOCKET_H_INCLUDED
#define POLYTEMPO_SOCKET_H_INCLUDED

#include "../Scheduler/Polytempo_EventObserver.h"

class Polytempo_Socket
{
public:
	Polytempo_Socket(const String& hostName_, int port_, bool ticks_ = false);
	~Polytempo_Socket();

	void renewBroadcaster();
	void write(OSCMessage oscMessage);
	bool transmitsTicks();

private:
	ScopedPointer < OSCSender > oscSender;
	ScopedPointer < String > hostName;
	int  port;
	bool ticks;
};



#endif  // POLYTEMPO_SOCKET_H_INCLUDED
