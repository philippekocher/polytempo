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
#include "../Misc/Polytempo_Alerts.h"
#include "../Application/PolytempoNetwork/Polytempo_NetworkApplication.h"



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
	Uuid senderId = Uuid((argumentIterator++)->getString());

	if (senderId == Polytempo_NetworkSupervisor::getInstance()->getUniqueId())
		return;

	if (addressPattern == "/node")
	{
		String argIp = (argumentIterator++)->getString();
		String argName = (argumentIterator++)->getString();

		Polytempo_NetworkSupervisor::getInstance()->addPeer(argIp, argName);
	}

#ifdef POLYTEMPO_NETWORK
	else if (addressPattern == "/open")
	{
		DBG("osc: open");
		// "Open Score": only PolytempoNetwork

		Polytempo_NetworkApplication* const app = dynamic_cast<Polytempo_NetworkApplication*>(JUCEApplication::getInstance());
		if ((*argumentIterator).isString())
			app->openScoreFilePath(argumentIterator->getString());
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

		ScopedPointer<Polytempo_Event> event = Polytempo_Event::makeEvent(addressPattern, *messages);
		Polytempo_ScoreScheduler::getInstance()->handleEvent(event, event->getTime());
	}
}
