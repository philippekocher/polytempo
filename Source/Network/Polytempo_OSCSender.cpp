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

#include "Polytempo_OSCSender.h"
#include "Polytempo_NetworkSupervisor.h"
#include "../../Resources/OSCPackLibrary/osc/OscOutboundPacketStream.h"


#define OUTPUT_BUFFER_SIZE 1024

Polytempo_OSCSender::Polytempo_OSCSender()
{
    socketsMap = new HashMap < String, Polytempo_Socket* >();
}

Polytempo_OSCSender::~Polytempo_OSCSender()
{
    deleteAll();
    broadcastSocket = nullptr;
}

void Polytempo_OSCSender::addBroadcastSender(int port)
{
    broadcastSocket = new Polytempo_Socket("169.254.255.255", port); // as well: 255.255.255.255
    Polytempo_NetworkSupervisor::getInstance()->setSocket(broadcastSocket);
}

void Polytempo_OSCSender::deleteAll()
{
    HashMap < String, Polytempo_Socket* >::Iterator it(*socketsMap);
    while(it.next()) { delete(it.getValue()); }
    socketsMap->clear();
}

void Polytempo_OSCSender::addSender(const juce::String &senderID, const juce::String &hostName, int port, bool ticks)
{
    Polytempo_Socket* socket = new Polytempo_Socket(hostName, port, ticks);
    socketsMap->set(senderID, socket);
}

void Polytempo_OSCSender::eventNotification(Polytempo_Event *event)
{
    if     (event->getType() == eventType_AddSender)
    {
        addSender(event->getProperty("senderID"), event->getProperty("ip"), event->getProperty("port"), event->getProperty("tick"));
    }
    else if(event->getType() == eventType_DeleteAll)
        deleteAll();
    else if(event->getType() == eventType_Osc)
        sendOscEventAsMessage(event);
    else if(event->getType() == eventType_Tick   ||
            event->getType() == eventType_Start  ||
            event->getType() == eventType_Stop)
        sendTick(event);
}

void Polytempo_OSCSender::broadcastEventAsMessage(Polytempo_Event *event)
{
    sendEventAsMessage(event, broadcastSocket);
}

void Polytempo_OSCSender::sendEventAsMessage(Polytempo_Event *event, Polytempo_Socket *socket)
{
    char buffer[OUTPUT_BUFFER_SIZE];
    memset(buffer, 0, OUTPUT_BUFFER_SIZE);
    osc::OutboundPacketStream p( buffer, OUTPUT_BUFFER_SIZE );
    
    p.Clear();

	String addressType = event->getOscAddressFromType();
    p << osc::BeginMessage(addressType.toRawUTF8());
    Array < var > messages = event->getOscMessageFromParameters();
    for(int i=0;i<messages.size();i++)
    {
        var element = messages[i];
        if(element.isInt())          p << int(element);
        //else if(element.isInt64())   p << osc::int64(int64(element));
        
        else if(element.isDouble())  p << float(element);
        else if(element.isString())  p << element.toString().toRawUTF8();
        //else                         p << "error";
    }
    p << osc::EndMessage;
    socket->write(buffer, OUTPUT_BUFFER_SIZE);
}

void Polytempo_OSCSender::sendOscEventAsMessage(Polytempo_Event *event)
{
    char buffer[OUTPUT_BUFFER_SIZE];
    memset(buffer, 0, OUTPUT_BUFFER_SIZE);
    osc::OutboundPacketStream p( buffer, OUTPUT_BUFFER_SIZE );
    
    p << osc::BeginMessage(event->getProperty("address").toString().toRawUTF8());
    var message = event->getProperty("message");
    Array < var > *messageArray;
    
    if(message.isArray())
    {
        messageArray = message.getArray();
    }
    else
    {
        messageArray = new Array < var >;
        messageArray->add(message);
    }
    
    for(int i=0;i<messageArray->size();i++)
    {
        var element = messageArray->getUnchecked(i);
        if(element.isInt())          p << int(element);
        //else if(element.isInt64())   p << osc::int64(int64(element));
        else if(element.isDouble())  p << double(element);
        else if(element.isString())  p << element.toString().toRawUTF8();
        else                         p << "error";
    }
    p << osc::EndMessage;
    
    Polytempo_Socket *socket = (*socketsMap)[event->getProperty("senderID")];
    if(socket != nullptr) socket->write(buffer, OUTPUT_BUFFER_SIZE);
}

void Polytempo_OSCSender::sendTick(Polytempo_Event *event)
{
    HashMap < String, Polytempo_Socket* >::Iterator it(*socketsMap);
    while(it.next())
    {
        Polytempo_Socket *socket = it.getValue();
        if(socket->transmitsTicks()) sendEventAsMessage(event, socket);
    }
}