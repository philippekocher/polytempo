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

#ifndef __Polytempo_OSCSender__
#define __Polytempo_OSCSender__

#include "../Scheduler/Polytempo_EventObserver.h"
#include "Polytempo_Socket.h"

class Polytempo_OSCSender : public Polytempo_EventObserver
{
public:
    Polytempo_OSCSender();
    ~Polytempo_OSCSender();
    
    void addBroadcastSender(int port);
    
    void eventNotification(Polytempo_Event *event);
    void broadcastEventAsMessage(Polytempo_Event *event);
    void addSender(const String& senderID, const String& hostName, int port, bool ticks = false);
    void deleteAll();
    void sendEventAsMessage(Polytempo_Event *event, Polytempo_Socket *socket);
    void sendOscEventAsMessage(Polytempo_Event *event);
    void sendTick(Polytempo_Event *event);

private:
    ScopedPointer < HashMap < String, Polytempo_Socket* > > socketsMap;
    ScopedPointer < Polytempo_Socket > broadcastSocket;
};


#endif // __Polytempo_OSCSender__
