/* ==============================================================================
 
 This file is part of the POLYTEMPO software package.
 Copyright (c) 2016 - Zurich University of the Arts,
 ICST Institute for Computer Music and Sound Technology
 http://www.icst.net
 
 Author: Philippe Kocher
 
 POLYTEMPO is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 POLYTEMPO is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this software. If not, see <http://www.gnu.org/licenses/>.
 
 ============================================================================== */


#include "Polytempo_NetworkSupervisor.h"
#include "Polytempo_IPAddress.h"
#include "../Preferences/Polytempo_StoredPreferences.h"
#include "Polytempo_NetworkInterfaceManager.h"
#include "Polytempo_TimeProvider.h"
#include "../Misc/Polytempo_Alerts.h"

Polytempo_NetworkSupervisor::Polytempo_NetworkSupervisor()
{
    oscSender = nullptr;
    component = nullptr;

	nodeName = new String(SystemStats::getFullUserName());

	startTimer(NETWORK_SUPERVISOR_PING_INTERVAL);
}

Polytempo_NetworkSupervisor::~Polytempo_NetworkSupervisor()
{
	stopTimer();

    localName = nullptr;
	nodeName = nullptr;
	oscSender = nullptr;

    clearSingletonInstance();
    
    Polytempo_NetworkInterfaceManager::deleteInstance();
}

juce_ImplementSingleton(Polytempo_NetworkSupervisor);

OSCMessage* Polytempo_NetworkSupervisor::createAdvertiseMessage(String ownIp)
{
	return new OSCMessage(
		OSCAddressPattern("/masteradvertise"),
		OSCArgument(getUniqueId().toString()),
		OSCArgument(ownIp));
}

void Polytempo_NetworkSupervisor::timerCallback()
{
	if(oscSender == nullptr || !Polytempo_TimeProvider::getInstance()->isMaster()) return;
	// nothing to do, if there is no socket or client not in Master mode
    
	Array<Polytempo_IPAddress> localIpAddresses;
	Polytempo_NetworkInterfaceManager::getInstance()->getAvailableIpAddresses(localIpAddresses);

	// broadcast a heartbeat
	for (Polytempo_IPAddress localIpAddress : localIpAddresses)
	{
		ScopedPointer<OSCMessage> msg = createAdvertiseMessage(localIpAddress.ipAddress.toString());
		oscSender->sendToIPAddress(localIpAddress.getBroadcastAddress().toString(), currentPort, *msg);
	}

	if(component) component->repaint();
}

Uuid Polytempo_NetworkSupervisor::getUniqueId()
{
	if (uniqueId == nullptr)
		uniqueId = Uuid();
	
	return uniqueId;
}

void Polytempo_NetworkSupervisor::unicastFlood(Polytempo_IPAddress ownIp)
{
	OSCSender localSender;
	localSender.connect(ownIp.ipAddress.toString(), 0);
	ScopedPointer<OSCMessage> msg = this->createAdvertiseMessage(ownIp.ipAddress.toString());
	IPAddress currentIp = ownIp.getFirstNetworkAddress();
	IPAddress lastIp = ownIp.getLastNetworkAddress();
	
	while (currentIp <= lastIp)
	{
		Logger::writeToLog("Sending node information to " + currentIp.toString());
		bool ok = localSender.sendToIPAddress(currentIp.toString(), currentPort, *msg);
		if(!ok)
		{
			Polytempo_Alert::show("Error", "Error sending node information to " + currentIp.toString());
			return;
		}
		// proceed to next address
		if (currentIp.address[3] == 255)
		{
			currentIp.address[3] = 0;
			if (currentIp.address[2] == 255)
			{
				currentIp.address[2] = 0;
				if (currentIp.address[1] == 255)
				{
					currentIp.address[1] = 0;
					currentIp.address[0]++;
				}
				else
					currentIp.address[1]++;
			}
			else
				currentIp.address[2]++;
		}
		else
			currentIp.address[3]++;
	}
}

String Polytempo_NetworkSupervisor::getDescription() const
{
	return getScoreName() + String(" (") + getPeerName() + String(")");
}

String Polytempo_NetworkSupervisor::getScoreName() const
{
	return String(localName == nullptr ? "Untitled" : *localName);
}

String Polytempo_NetworkSupervisor::getPeerName() const
{
	return String(*nodeName);
}

void Polytempo_NetworkSupervisor::createSender(int port)
{
	currentPort = port;
	oscSender = new OSCSender();
	oscSender->connect("255.255.255.255", currentPort);
}

void Polytempo_NetworkSupervisor::setComponent(Component *aComponent)
{
	component = aComponent;
}

void Polytempo_NetworkSupervisor::eventNotification(Polytempo_Event *event)
{
	if(event->getType() == eventType_Settings)
	{
		localName = new String(event->getProperty("name").toString());
	}
}