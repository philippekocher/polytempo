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

#include "Polytempo_OSCListener.h"
#include "Polytempo_NetworkSupervisor.h"
#include "../../Resources/OSCPackLibrary/osc/OscReceivedElements.h"
#include "../Scheduler/Polytempo_Scheduler.h"
#include "../Misc/Polytempo_Alerts.h"
#include "../Application/PolytempoNetwork/Polytempo_NetworkApplication.h"



Polytempo_OSCListener::Polytempo_OSCListener(int port) : Thread ("OSC_Listener Thread"), m_Port(port)
{}

Polytempo_OSCListener::~Polytempo_OSCListener()
{
    signalThreadShouldExit();
    
   // if (socket != 0)
   //     socket->close();
    
    // allow the thread 2 seconds to stop cleanly - should be plenty of time.
    stopThread (2000);
    
    messageData = nullptr;
}

void Polytempo_OSCListener::handleMessage(const Message& message)
{
    if(true) //message.intParameter1 == 1) // 1 means I've received stuff.
    {        
        int length = ((PolytempoOSCMessage *)&message)->length;
        
        MemoryBlock* data (static_cast <MemoryBlock*> (((PolytempoOSCMessage *)&message)->data));
                
        osc::ReceivedMessage m = osc::ReceivedMessage(osc::ReceivedPacket((char *)data->getData(),length));
        
        if(std::strcmp(m.AddressPattern(), "/node" ) == 0)
        {
            osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
            Polytempo_NetworkSupervisor::getInstance()->addPeer(arg->AsString(),(++arg)->AsString());
        }
        
#ifdef POLYTEMPO_NETWORK
        else if(std::strcmp(m.AddressPattern(), "/open" ) == 0)
        {
            DBG("osc: open");
            // "Open Score": only PolytempoNetwork
            
            Polytempo_NetworkApplication* const app = dynamic_cast<Polytempo_NetworkApplication*>(JUCEApplication::getInstance());
            osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
            if(arg != m.ArgumentsEnd())
                app->openScoreFilePath(arg->AsString());
        }
#endif
        else
        {
            Array<var> *messages = new Array<var>();

            const char *address = m.AddressPattern();
            DBG("osc: "<<address);
            
            osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
            while(arg != m.ArgumentsEnd())
            {
                if((*arg).IsString())
                    messages->add(var(String((arg)->AsString())));
//                else if((*arg).IsInt64())
//                    messages->add(var((int64)(arg)->AsInt64()));
                else if((*arg).IsInt32())
                    messages->add(var((int32)(arg)->AsInt32()));
                else if((*arg).IsFloat())
                    messages->add(var((float)(arg)->AsFloat()));
                else if((*arg).IsDouble())
                    messages->add(var((double)(arg)->AsDouble()));
                else if((*arg).IsBlob())
                    DBG("<blob>");
                else
                    DBG("<unknown>");

                arg++;                
            }
            
            Polytempo_Event *event = Polytempo_Event::makeEvent(address, *messages);
            Polytempo_Scheduler::getInstance()->handleEvent(event, event->getTime());
        }
    }
}

void Polytempo_OSCListener::run()
{
    messageData = new MemoryBlock(65535, true);
    socket = new DatagramSocket(-1);
    
    int readyForReading;
    int bytesRead;
    
    String senderIPAddress;
    int senderPort;
        
    if(!socket->bindToPort(m_Port))
    {
        Polytempo_Alert::show("Error", "Can't bind to port: " + String(m_Port)+"\nProbably there is another socket already bound to this port");
    }
    
    while(!threadShouldExit())
    {
        readyForReading = socket->waitUntilReady(true, 1000);
        
        if(readyForReading > 0)
        {
            bytesRead = socket->read(static_cast <char*> (messageData->getData()), 65535, false, senderIPAddress, senderPort);
            
            if(senderIPAddress != Polytempo_NetworkSupervisor::getInstance()->getLocalAddress())
            {
                PolytempoOSCMessage *message = new PolytempoOSCMessage();
            
                message->setData(bytesRead, new MemoryBlock(*messageData));
                postMessage(message);
            }
        }
    }
}
