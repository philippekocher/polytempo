/*
  ==============================================================================

    Polytempo_NetworkInterfaceManager.cpp
    Created: 12 Feb 2017 11:25:07am
    Author:  christian.schweizer

  ==============================================================================
*/

#include "Polytempo_NetworkInterfaceManager.h"
#include "../Preferences/Polytempo_StoredPreferences.h"

Polytempo_NetworkInterfaceManager::Polytempo_NetworkInterfaceManager()
{
	selectedIpAddress = Polytempo_IPAddress();

	updateAddresses();

	String lastAddress = Polytempo_StoredPreferences::getInstance()->getProps().getValue("selectedIP", String::empty);
	if (lastAddress != String::empty)
	{
		for (Polytempo_IPAddress ip : availableIpAddresses)
		{
			if (ip.ipAddress.toString() == lastAddress)
			{
				selectedIpAddress = ip;
				break;
			}
		}
	}
	else
	{
		for (Polytempo_IPAddress ip : availableIpAddresses)
		{
			if (ip.ipAddress.address[0] == 192)
			{
				selectedIpAddress = ip;
				break;
			}
		}
	}

	startTimer(500);
}

Polytempo_NetworkInterfaceManager::~Polytempo_NetworkInterfaceManager()
{
	stopTimer();
	
	clearSingletonInstance();
}

juce_ImplementSingleton(Polytempo_NetworkInterfaceManager);

int Polytempo_NetworkInterfaceManager::getAvailableIpAddresses(Array<Polytempo_IPAddress>& ipAdresses)
{
	ipAdresses.clear();
	ipAdresses.addArray(availableIpAddresses);
	return ipAdresses.size();
}

Polytempo_IPAddress Polytempo_NetworkInterfaceManager::getSelectedIpAddress()
{
	return selectedIpAddress;
}

void Polytempo_NetworkInterfaceManager::setSelectedIpAddress(Polytempo_IPAddress ip)
{
	if (availableIpAddresses.contains(ip))
	{
		selectedIpAddress = ip;
		sendChangeMessage();
		Polytempo_StoredPreferences::getInstance()->getProps().setValue("selectedIP", ip.ipAddress.toString());
	}
}

void Polytempo_NetworkInterfaceManager::timerCallback()
{
	if (updateAddresses())
	{
		if (!availableIpAddresses.contains(selectedIpAddress))
		{
			selectedIpAddress = Polytempo_IPAddress();
		}
		sendChangeMessage();
	}
}

bool Polytempo_NetworkInterfaceManager::updateAddresses()
{
	Array<Polytempo_IPAddress> detectedIPs;
	Polytempo_IPAddress::findAllAddresses(detectedIPs);

	if (detectedIPs != availableIpAddresses)
	{
		availableIpAddresses = detectedIPs;
		return true;
	}
	return false;
}
