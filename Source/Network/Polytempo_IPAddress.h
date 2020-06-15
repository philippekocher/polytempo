#pragma once

#include "JuceHeader.h"

class Polytempo_IPAddress
{
public:
    static void findAllAddresses(Array<Polytempo_IPAddress>& results);
    static Polytempo_IPAddress local() noexcept;

    String addressDescription();
    IPAddress getBroadcastAddress();
    IPAddress getNetworkAddress();
    IPAddress getFirstNetworkAddress();
    IPAddress getLastNetworkAddress();

    Polytempo_IPAddress(IPAddress ipAdress, IPAddress subnetMask, String adapterName);
    Polytempo_IPAddress() noexcept;

    bool operator==(const Polytempo_IPAddress& other) const noexcept;
    bool operator!=(const Polytempo_IPAddress& other) const noexcept;

    IPAddress ipAddress;
    IPAddress subnetMask;
    String adapterName;
};
