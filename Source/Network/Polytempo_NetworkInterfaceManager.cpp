/*
  ==============================================================================

    Polytempo_NetworkInterfaceManager.cpp
    Created: 12 Feb 2017 11:25:07am
    Author:  christian.schweizer

  ==============================================================================
*/

#include "Polytempo_NetworkInterfaceManager.h"
#include "../Preferences/Polytempo_StoredPreferences.h"

bool Polytempo_NetworkInterfaceManager::TrySelectIpWithFirstNumber(uint8 number)
{
	bool found = false;

	for (Polytempo_IPAddress ip : availableIpAddresses)
	{
		if (ip.ipAddress.address[0] == number)
		{
			selectedIpAddress = ip;
			found = true;
			break;
		}
	}

	return found;
}

bool Polytempo_NetworkInterfaceManager::TrySelectLastUsedAdapter()
{
	bool found = false;

	updateAddresses();

	String lastNetworkAdapter = Polytempo_StoredPreferences::getInstance()->getProps().getValue("selectedNetworkAdapter", String::empty);
	if (lastNetworkAdapter != String::empty)
	{
		for (Polytempo_IPAddress ip : availableIpAddresses)
		{
			if (ip.adapterName == lastNetworkAdapter)
			{
				selectedIpAddress = ip;
				found = true;
				break;
			}
		}
	}

	return found;
}

void Polytempo_NetworkInterfaceManager::timerCallback()
{
	bool found = TrySelectLastUsedAdapter();
	if (found)
		stopTimer();
}

Polytempo_NetworkInterfaceManager::Polytempo_NetworkInterfaceManager()
{
	selectedIpAddress = Polytempo_IPAddress();

	bool adapterLock = Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("networkAdapterLock", false);
	bool found = TrySelectLastUsedAdapter();
	if(!found && adapterLock)
	{
		startTimer(1000);
	}
		
	if (!found)
	{
		found = TrySelectIpWithFirstNumber(192);
	}
	
	if (!found)
	{
		found = TrySelectIpWithFirstNumber(169);
	}

	if (!found)
	{
		if (availableIpAddresses.size() > 0)
			selectedIpAddress = availableIpAddresses[0];
	}

	if(found)
	{
		if (!adapterLock)
		{
			Polytempo_StoredPreferences::getInstance()->getProps().setValue("selectedNetworkAdapter", selectedIpAddress.adapterName);
		}
	}
}

Polytempo_NetworkInterfaceManager::~Polytempo_NetworkInterfaceManager()
{
	clearSingletonInstance();
}

juce_ImplementSingleton(Polytempo_NetworkInterfaceManager);

int Polytempo_NetworkInterfaceManager::getAvailableIpAddresses(Array<Polytempo_IPAddress>& ipAdresses)
{
	ipAdresses.clear();
	updateAddresses();
	ipAdresses.addArray(availableIpAddresses);
	return ipAdresses.size();
}

Polytempo_IPAddress Polytempo_NetworkInterfaceManager::getSelectedIpAddress() const
{
	return selectedIpAddress;
}

void Polytempo_NetworkInterfaceManager::setSelectedIpAddress(Polytempo_IPAddress ip)
{
	if (availableIpAddresses.contains(ip))
	{
		selectedIpAddress = ip;
		Polytempo_StoredPreferences::getInstance()->getProps().setValue("selectedNetworkAdapter", ip.adapterName);
	}
}

bool Polytempo_NetworkInterfaceManager::updateAddresses()
{
	Array<Polytempo_IPAddress> detectedIPs;
	Polytempo_IPAddress::findAllAddresses(detectedIPs);

	if (detectedIPs != availableIpAddresses)
	{
		if (!availableIpAddresses.contains(selectedIpAddress))
		{
			selectedIpAddress = Polytempo_IPAddress();
		}
		availableIpAddresses = detectedIPs;
		return true;
	}
	return false;
}
