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
    connectedPeersMap = new HashMap < Uuid, Polytempo_PeerInfo >();
    
    socket = nullptr;
    component = nullptr;

	nodeName = new String(SystemStats::getFullUserName());

	startTimer(NETWORK_SUPERVISOR_PING_INTERVAL);
}

Polytempo_NetworkSupervisor::~Polytempo_NetworkSupervisor()
{
	stopTimer();

    connectedPeersMap = nullptr;
	localName = nullptr;
	nodeName = nullptr;
	socket = nullptr;

    clearSingletonInstance();
    
    Polytempo_NetworkInterfaceManager::deleteInstance();
}

juce_ImplementSingleton(Polytempo_NetworkSupervisor);

OSCMessage* Polytempo_NetworkSupervisor::createNodeMessage()
{
	return new OSCMessage(
		OSCAddressPattern("/node"),
		OSCArgument(getUniqueId().toString()),
		OSCArgument(Polytempo_NetworkInterfaceManager::getInstance()->getSelectedIpAddress().ipAddress.toString()),
		OSCArgument(String(getScoreName())),
		OSCArgument(String(getPeerName())),
		OSCArgument((int32)Polytempo_TimeProvider::getInstance()->isMaster()),
		OSCArgument((int32)Polytempo_TimeProvider::getInstance()->getDelaySafeTimestamp()));
}

void Polytempo_NetworkSupervisor::timerCallback()
{
	if(socket == nullptr) return;
	// nothing to do, if there is no socket
    
	socket->renewBroadcaster();
    
	// broadcast a heartbeat
	ScopedPointer<OSCMessage> msg = createNodeMessage();
	socket->write(*msg);
	
	if(component) component->repaint();
}

String Polytempo_NetworkSupervisor::getAdapterInfo()
{
	Polytempo_IPAddress ip = Polytempo_NetworkInterfaceManager::getInstance()->getSelectedIpAddress();
	return ip.addressDescription() + "\r\n" + ip.getNetworkAddress().toString() + "\r\n(" + ip.ipAddress.toString() + ")";
}

Uuid Polytempo_NetworkSupervisor::getUniqueId()
{
	if (uniqueId != nullptr)
		return uniqueId;

	String uuidStr = Polytempo_StoredPreferences::getInstance()->getProps().getValue("uniqueId", String());
	if(uuidStr != String())
		uniqueId = Uuid(uuidStr);
	else
	{
		// create new UUID
		uniqueId = Uuid();
		Polytempo_StoredPreferences::getInstance()->getProps().setValue("uniqueId", uniqueId.toString());
	}
	return uniqueId;
}

void Polytempo_NetworkSupervisor::manualConnect(String ip)
{
	OSCSender localSender;
	localSender.connect(Polytempo_NetworkInterfaceManager::getInstance()->getSelectedIpAddress().ipAddress.toString(), 0);
	ScopedPointer<OSCMessage> msg = this->createNodeMessage();
	Logger::writeToLog("Sending node information to " + ip);
	bool ok = localSender.sendToIPAddress(ip, currentPort, *msg);
	if(!ok)
	{
		Polytempo_Alert::show("Error", "Error sending node information to " + ip);
	}
}

void Polytempo_NetworkSupervisor::unicastFlood()
{
	OSCSender localSender;
	localSender.connect(Polytempo_NetworkInterfaceManager::getInstance()->getSelectedIpAddress().ipAddress.toString(), 0);
	ScopedPointer<OSCMessage> msg = this->createNodeMessage();
	IPAddress currentIp = Polytempo_NetworkInterfaceManager::getInstance()->getSelectedIpAddress().getFirstNetworkAddress();
	IPAddress lastIp = Polytempo_NetworkInterfaceManager::getInstance()->getSelectedIpAddress().getLastNetworkAddress();
	
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

HashMap <Uuid, Polytempo_PeerInfo>* Polytempo_NetworkSupervisor::getPeers() const
{
	return connectedPeersMap;
}

void Polytempo_NetworkSupervisor::createSocket(int port)
{
	currentPort = port;
	socket = new Polytempo_Socket("255.255.255.255", port); // dummy broadcaster, will be overwritten by renewBroadcaster()
}

void Polytempo_NetworkSupervisor::setComponent(Component *aComponent)
{
	component = aComponent;
}

void Polytempo_NetworkSupervisor::handlePeer(Uuid id, String ip, String scoreName, String peerName, bool syncOk) const
{
	Polytempo_PeerInfo info;
	info.ip = ip;
	info.scoreName = scoreName;
	info.peerName = peerName;
	info.lastHeartBeat = Time::getMillisecondCounter();
	info.syncState = syncOk;

	connectedPeersMap->set(id, info);
}

void Polytempo_NetworkSupervisor::resetPeers() const
{
	connectedPeersMap->clear();
}

void Polytempo_NetworkSupervisor::eventNotification(Polytempo_Event *event)
{
	if(event->getType() == eventType_Settings)
	{
		localName = new String(event->getProperty("name").toString());
	}
}


