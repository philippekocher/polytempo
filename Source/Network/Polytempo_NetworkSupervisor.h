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

#ifndef __Polytempo_NetworkSupervisor__
#define __Polytempo_NetworkSupervisor__

#include "../Scheduler/Polytempo_EventObserver.h"
#include "Polytempo_Socket.h"
#include "Polytempo_BroadcastWrapper.h"
#include "Polytempo_PeerInfo.h"
#define NETWORK_SUPERVISOR_PING_INTERVAL	1000

class Polytempo_NetworkSupervisor : public Timer,
                                    public Polytempo_EventObserver
{
public:
	juce_DeclareSingleton(Polytempo_NetworkSupervisor, false);

	Polytempo_NetworkSupervisor();
    ~Polytempo_NetworkSupervisor();

	void timerCallback() override;

	static String getAdapterInfo();
    String getDescription() const;
	String getScoreName() const; 
	String getPeerName() const;
	HashMap <Uuid, Polytempo_PeerInfo>* getPeers() const;
    void createSocket(int port);
    void setComponent(Component *aComponent);
	void setBroadcastSender(Polytempo_BroadcastWrapper* pBroadcastWrapper);
    void handlePeer(Uuid id, String ip, String scoreName, String peerName, bool syncOk) const;

    void eventNotification(Polytempo_Event *event) override;
	Uuid getUniqueId();
	void manualConnect(String ip);
	void unicastFlood();

private:
	OSCMessage* createNodeMessage();

	Uuid uniqueId = nullptr;
    ScopedPointer<Polytempo_Socket> socket;
    Component *component;
    
    ScopedPointer < String > localName;
	ScopedPointer < String > nodeName;

    ScopedPointer < HashMap < Uuid, Polytempo_PeerInfo > > connectedPeersMap;

	Polytempo_BroadcastWrapper* pBroadcastWrapper;
	int currentPort;
};


#endif  // __Polytempo_NetworkSupervisor__
