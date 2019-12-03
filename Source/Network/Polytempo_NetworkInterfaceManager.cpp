#include "Polytempo_NetworkInterfaceManager.h"
#include "../Preferences/Polytempo_StoredPreferences.h"

Polytempo_NetworkInterfaceManager::Polytempo_NetworkInterfaceManager()
{
}

Polytempo_NetworkInterfaceManager::~Polytempo_NetworkInterfaceManager()
{
    clearSingletonInstance();
}

juce_ImplementSingleton(Polytempo_NetworkInterfaceManager);

int Polytempo_NetworkInterfaceManager::getAvailableIpAddresses(Array<Polytempo_IPAddress>& ipAdresses)
{
    Array<Polytempo_IPAddress> detectedIPs;

    ipAdresses.clear();
    Polytempo_IPAddress::findAllAddresses(detectedIPs);
    if (detectedIPs != availableIpAddresses)
    {
        availableIpAddresses = detectedIPs;
    }

    ipAdresses.addArray(availableIpAddresses);
    return ipAdresses.size();
}
