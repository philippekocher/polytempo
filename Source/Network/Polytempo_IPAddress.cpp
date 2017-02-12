/*
  ==============================================================================

    Polytempo_IPAddress.cpp
    Created: 12 Feb 2017 10:05:45am
    Author:  christian.schweizer

  ==============================================================================
*/

#include "Polytempo_IPAddress.h"

Polytempo_IPAddress::Polytempo_IPAddress() noexcept
{
	ipAddress = IPAddress();
	subnetMask = IPAddress();
}

Polytempo_IPAddress::Polytempo_IPAddress(IPAddress address, IPAddress mask)
{
	ipAddress = address;
	subnetMask = mask;
}

bool Polytempo_IPAddress::operator== (const Polytempo_IPAddress& other) const noexcept
{
	return ipAddress == other.ipAddress && subnetMask == other.subnetMask;
}

bool Polytempo_IPAddress::operator!= (const Polytempo_IPAddress& other) const noexcept
{
	return !operator== (other);
}

Polytempo_IPAddress Polytempo_IPAddress::local() noexcept { return Polytempo_IPAddress(IPAddress::local(), IPAddress(255, 0, 0, 0)); }

String Polytempo_IPAddress::addressDescription()
{
	if (ipAddress == IPAddress(127, 0, 0, 1))
		return "Localhost";
	if (ipAddress.address[0] == 192 && ipAddress.address[1] == 168)
		return "Private local network";
	if (ipAddress.address[0] == 172 && ipAddress.address[1] >= 16 && ipAddress.address[1] <= 31)
		return "Private local network";
	if (ipAddress.address[0] == 10)
		return "Private local network";
	if (ipAddress.address[1] == 169 && ipAddress.address[1] == 254)
		return "Link-local network";

	return "Other network";
}

IPAddress Polytempo_IPAddress::getBroadcastAddress()
{
	IPAddress networkAddress = getNetworkAddress();
	int8 broadcastAddress[4];
	for (int i = 0; i < 4; i++)
	{
		broadcastAddress[i] = networkAddress.address[i] | ~subnetMask.address[i];
	}
	return IPAddress(broadcastAddress[0], broadcastAddress[1], broadcastAddress[2], broadcastAddress[3]);
}

IPAddress Polytempo_IPAddress::getNetworkAddress()
{
	int8 networkAddress[4];
	for (int i = 0; i < 4; i++)
	{
		networkAddress[i] = ipAddress.address[i] & subnetMask.address[i];
	}
	return IPAddress(networkAddress[0], networkAddress[1], networkAddress[2], networkAddress[3]);
}


#if JUCE_WINDOWS
struct GetAdaptersInfoHelper
{
	bool callGetAdaptersInfo()
	{
		DynamicLibrary dll("iphlpapi.dll");
		JUCE_LOAD_WINAPI_FUNCTION(dll, GetAdaptersInfo, getAdaptersInfo, DWORD, (PIP_ADAPTER_INFO, PULONG))

			if (getAdaptersInfo == nullptr)
				return false;

		adapterInfo.malloc(1);
		ULONG len = sizeof(IP_ADAPTER_INFO);

		if (getAdaptersInfo(adapterInfo, &len) == ERROR_BUFFER_OVERFLOW)
			adapterInfo.malloc(len, 1);

		return getAdaptersInfo(adapterInfo, &len) == NO_ERROR;
	}

	HeapBlock<IP_ADAPTER_INFO> adapterInfo;
};


void Polytempo_IPAddress::findAllAddresses(Array<Polytempo_IPAddress>& result)
{
	result.addIfNotAlreadyThere(Polytempo_IPAddress(Polytempo_IPAddress::local()));

	GetAdaptersInfoHelper gah;

	if (gah.callGetAdaptersInfo())
	{
		for (PIP_ADAPTER_INFO adapter = gah.adapterInfo; adapter != nullptr; adapter = adapter->Next)
		{
			Polytempo_IPAddress ip(IPAddress(adapter->IpAddressList.IpAddress.String), IPAddress(adapter->IpAddressList.IpMask.String));

			if (ip.ipAddress != IPAddress::any())
				result.addIfNotAlreadyThere(ip);
		}
	}
}


#else
static void addAddress(const sockaddr_in* addr_in, Array<Polytempo_IPAddress>& result)
{
	in_addr_t addr = addr_in->sin_addr.s_addr;

	if (addr != INADDR_NONE)
		result.addIfNotAlreadyThere(Polytempo_IPAddress(IPAddress(ntohl(addr)), nullptr);
}

static void findIPAddresses(int sock, Array<Polytempo_IPAddress>& result)
{
	ifconf cfg;
	HeapBlock<char> buffer;
	int bufferSize = 1024;

	do
	{
		bufferSize *= 2;
		buffer.calloc((size_t)bufferSize);

		cfg.ifc_len = bufferSize;
		cfg.ifc_buf = buffer;

		if (ioctl(sock, SIOCGIFCONF, &cfg) < 0 && errno != EINVAL)
			return;

	} while (bufferSize < cfg.ifc_len + 2 * (int)(IFNAMSIZ + sizeof(struct sockaddr_in6)));

#if JUCE_MAC || JUCE_IOS
	while (cfg.ifc_len >= (int)(IFNAMSIZ + sizeof(struct sockaddr_in)))
	{
		if (cfg.ifc_req->ifr_addr.sa_family == AF_INET) // Skip non-internet addresses
			addAddress((const sockaddr_in*)&cfg.ifc_req->ifr_addr, result);

		cfg.ifc_len -= IFNAMSIZ + cfg.ifc_req->ifr_addr.sa_len;
		cfg.ifc_buf += IFNAMSIZ + cfg.ifc_req->ifr_addr.sa_len;
	}
#else
	for (size_t i = 0; i < (size_t)cfg.ifc_len / (size_t) sizeof(struct ifreq); ++i)
	{
		const ifreq& item = cfg.ifc_req[i];

		if (item.ifr_addr.sa_family == AF_INET)
			addAddress((const sockaddr_in*)&item.ifr_addr, result);
	}
#endif
}

void Polytempo_IPAddress::findAllAddresses(Array<Polytempo_IPAddress>& result)
{
	const int sock = socket(AF_INET, SOCK_DGRAM, 0); // a dummy socket to execute the IO control

	if (sock >= 0)
	{
		findIPAddresses(sock, result);
		::close(sock);
	}
}
#endif