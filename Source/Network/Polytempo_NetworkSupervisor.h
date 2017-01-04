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

//#include "../../JuceLibraryCode/JuceHeader.h"
#include "../Scheduler/Polytempo_EventObserver.h"

#include "Polytempo_OSCSender.h"


class Polytempo_NetworkSupervisor : public Timer,
                                    public Polytempo_EventObserver
{
public:
    Polytempo_NetworkSupervisor();
    ~Polytempo_NetworkSupervisor();
    
    juce_DeclareSingleton(Polytempo_NetworkSupervisor, false);
    
    void timerCallback();
    
    String getLocalAddress();
    String getLocalName();
    HashMap <String, String>* getPeers();
    void setSocket(Polytempo_Socket *aSocket);
    void setComponent(Component *aComponent);

    void addPeer(String name, String ip);

    void eventNotification(Polytempo_Event *event);

private:
    Polytempo_Socket *socket;
    Component *component;
    
    ScopedPointer <String> localAddress = new String("0.0.0.0"); // this computer's ip address
    ScopedPointer < String > localName;

    ScopedPointer < HashMap < String, String > > connectedPeersMap;
    ScopedPointer < HashMap < String, String > > tempConnectedPeersMap;

};


#endif  // __Polytempo_NetworkSupervisor__
