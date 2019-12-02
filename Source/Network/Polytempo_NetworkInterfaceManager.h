#ifndef POLYTEMPO_NETWORKINTERFACEMANAGER_H_INCLUDED
#define POLYTEMPO_NETWORKINTERFACEMANAGER_H_INCLUDED

#include "Polytempo_IPAddress.h"

class Polytempo_NetworkInterfaceManager
{
public:
	Polytempo_NetworkInterfaceManager();
	~Polytempo_NetworkInterfaceManager();
	
	juce_DeclareSingleton(Polytempo_NetworkInterfaceManager, false);

	int getAvailableIpAddresses(Array<Polytempo_IPAddress>& ipAdresses);
	
private:
	Array<Polytempo_IPAddress> availableIpAddresses;
};



#endif  // POLYTEMPO_NETWORKINTERFACEMANAGER_H_INCLUDED
