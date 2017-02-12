/*
  ==============================================================================

    Polytempo_NetworkInterfaceManager.h
    Created: 12 Feb 2017 11:25:07am
    Author:  christian.schweizer

  ==============================================================================
*/

#ifndef POLYTEMPO_NETWORKINTERFACEMANAGER_H_INCLUDED
#define POLYTEMPO_NETWORKINTERFACEMANAGER_H_INCLUDED

#include "Polytempo_IPAddress.h"

class Polytempo_NetworkInterfaceManager :	public Timer, 
											public ChangeBroadcaster
{
public:
	Polytempo_NetworkInterfaceManager();
	~Polytempo_NetworkInterfaceManager();

	juce_DeclareSingleton(Polytempo_NetworkInterfaceManager, false);

	int getAvailableIpAddresses(Array<Polytempo_IPAddress>& ipAdresses);
	Polytempo_IPAddress getSelectedIpAddress();
	void setSelectedIpAddress(Polytempo_IPAddress ip);

private:
	void timerCallback();
	bool updateAddresses();

private:
	Array<Polytempo_IPAddress> availableIpAddresses;
	Polytempo_IPAddress selectedIpAddress;
};



#endif  // POLYTEMPO_NETWORKINTERFACEMANAGER_H_INCLUDED
