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

#define PING_INTERVAL	1000

Polytempo_NetworkSupervisor::Polytempo_NetworkSupervisor()
{
    connectedPeersMap = new HashMap < String, String >();
    
    socket = nullptr;
    component = nullptr;

	nodeName = new String(SystemStats::getFullUserName());

	pBroadcastWrapper = nullptr;

	startTimer(PING_INTERVAL);
}

Polytempo_NetworkSupervisor::~Polytempo_NetworkSupervisor()
{
	stopTimer();

    connectedPeersMap = nullptr;
	localName = nullptr;
	nodeName = nullptr;

    clearSingletonInstance();
    
    Polytempo_NetworkInterfaceManager::deleteInstance();
}

juce_ImplementSingleton(Polytempo_NetworkSupervisor);

void Polytempo_NetworkSupervisor::timerCallback()
{
    if(socket == nullptr) return;
    // nothing to do, if there is no socket
    
    socket->renewBroadcaster();
    
    // broadcast a heartbeat
    ScopedPointer<String> name = new String(getLocalName());
    
	OSCMessage msg = OSCMessage(
		OSCAddressPattern("/node"),
		OSCArgument(getUniqueId().toString()),
		OSCArgument(Polytempo_NetworkInterfaceManager::getInstance()->getSelectedIpAddress().ipAddress.toString()),
		OSCArgument(*name),
		OSCArgument((int32)Polytempo_TimeProvider::getInstance()->isMaster()));

	socket->write(msg);
	
	if(pBroadcastWrapper != nullptr)
	{
		pBroadcastWrapper->SendOsc(&msg);
	}

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

String Polytempo_NetworkSupervisor::getLocalName()
{
	return String(localName == nullptr ? "Untitled" : *localName) + String(" (") + String(*nodeName) + String(")");
}

HashMap <String, String>* Polytempo_NetworkSupervisor::getPeers()
{
    return connectedPeersMap;
}

void Polytempo_NetworkSupervisor::setSocket(Polytempo_Socket *theSocket)
{
    socket = theSocket;
}

void Polytempo_NetworkSupervisor::setComponent(Component *aComponent)
{
    component = aComponent;
}

void Polytempo_NetworkSupervisor::setBroadcastWrapper(Polytempo_BroadcastWrapper* pBroadcaster)
{
	pBroadcastWrapper = pBroadcaster;
}

void Polytempo_NetworkSupervisor::handlePeer(String ip, String name)
{
	connectedPeersMap->set(ip, name);
}

void Polytempo_NetworkSupervisor::eventNotification(Polytempo_Event *event)
{
    if(event->getType() == eventType_Settings)
    {
        localName = new String(event->getProperty("name").toString());
    }
}


