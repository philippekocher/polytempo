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
#include "../Scheduler/Polytempo_ScoreScheduler.h"
#include "../Scheduler/Polytempo_EventScheduler.h"
#include "../Misc/Polytempo_Alerts.h"
#include "../Application/PolytempoNetwork/Polytempo_NetworkApplication.h"
#include "Polytempo_TimeProvider.h"


Polytempo_OSCListener::Polytempo_OSCListener(int port) : m_Port(port)
{
	oscReceiver = new OSCReceiver();

	if (!oscReceiver->connect(m_Port))
		Polytempo_Alert::show("Error", "Can't bind to port: " + String(m_Port) + "\nProbably there is another socket already bound to this port");

	oscReceiver->addListener(this);
}

Polytempo_OSCListener::~Polytempo_OSCListener()
{
}

void Polytempo_OSCListener::oscMessageReceived(const OSCMessage & message)
{
	String addressPattern = message.getAddressPattern().toString();
	OSCArgument* argumentIterator = message.begin();

	if (addressPattern == "/node")
	{
		Uuid senderId = Uuid((argumentIterator++)->getString());		
		if (senderId == Polytempo_NetworkSupervisor::getInstance()->getUniqueId())
			return;

		String argIp = (argumentIterator++)->getString();
		String argName = (argumentIterator++)->getString();
		
		bool isMaster = false;
		if (argumentIterator && (*argumentIterator).isInt32())
			isMaster = (bool)argumentIterator->getInt32();

		bool syncOk = false;
		if (argumentIterator && (*argumentIterator).isInt32())
		{
			uint32 syncTime;
			syncOk = Polytempo_TimeProvider::getInstance()->getSyncTime(&syncTime) && uint32(argumentIterator->getInt32()) <= syncTime;
		}

		Polytempo_NetworkSupervisor::getInstance()->handlePeer(senderId, argIp, argName, syncOk);
		Polytempo_TimeProvider::getInstance()->setRemoteMasterPeer(argIp, senderId, isMaster);
	}

#ifdef POLYTEMPO_NETWORK
	else if (addressPattern == "/open")
	{
		const MessageManagerLock mml(Thread::getCurrentThread());

		DBG("osc: open");
		// "Open Score": only PolytempoNetwork

		Polytempo_NetworkApplication* const app = dynamic_cast<Polytempo_NetworkApplication*>(JUCEApplication::getInstance());
		if (argumentIterator != message.end() && (*argumentIterator).isString())
        {
            String filePath(argumentIterator->getString());
            if(filePath.startsWithChar(File::getSeparatorChar()) ||
               filePath.startsWithChar('~'))
                app->openScoreFilePath(argumentIterator->getString());
        }
	}
    else if (addressPattern == "/fullScreen")
    {
		const MessageManagerLock mml(Thread::getCurrentThread());

        Polytempo_NetworkApplication* const app = dynamic_cast<Polytempo_NetworkApplication*>(JUCEApplication::getInstance());

        Polytempo_NetworkWindow *window = app->getMainWindow();
        if (argumentIterator != message.end() && (*argumentIterator).isInt32())
        {
            if (argumentIterator->getInt32() > 0) window->setFullScreen(true);
            else                                  window->setFullScreen(false);
        }
        else window->setFullScreen(true);
    }
#endif
	else
	{
		ScopedPointer<Array<var>> messages = new Array<var>();

		DBG("osc: " << addressPattern);

		while (argumentIterator != message.end())
		{
			if ((*argumentIterator).isString())
				messages->add(var(String((argumentIterator)->getString())));
			else if ((*argumentIterator).isInt32())
				messages->add(var(int32((argumentIterator)->getInt32())));
			else if ((*argumentIterator).isFloat32())
				messages->add(var(float((argumentIterator)->getFloat32())));
			else if ((*argumentIterator).isBlob())
				DBG("<blob>");
			else
				DBG("<unknown>");

			argumentIterator++;
		}

		Polytempo_Event* event = Polytempo_Event::makeEvent(addressPattern, *messages);
        
        if(event->getType() == eventType_None)
        {
            DBG("--unknown");
            delete event;
            return;
        }
        
        // calculate syncTime
        uint32 syncTime;
        
        if(event->hasProperty(eventPropertyString_TimeTag))
            syncTime = uint32(int32(event->getProperty(eventPropertyString_TimeTag)));
		else
		{
			Polytempo_TimeProvider::getInstance()->getSyncTime(&syncTime);
		}

        if(event->hasProperty(eventPropertyString_Time))
        {
            Polytempo_ScoreScheduler *scoreScheduler = Polytempo_ScoreScheduler::getInstance();
            
            if(!scoreScheduler->isRunning()) return; // ignore an event that has a "time"
                                                     // when the score scheduler isn't running
        
            syncTime += event->getTime() - scoreScheduler->getScoreTime();
        }
        
        if(event->hasProperty(eventPropertyString_Defer))
            syncTime += int(float(event->getProperty(eventPropertyString_Defer)) * 1000.0f);
        
        event->setSyncTime(syncTime);

		Polytempo_EventScheduler::getInstance()->scheduleEvent(event);
	}
}
