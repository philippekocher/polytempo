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
#include "../Scheduler/Polytempo_Scheduler.h"
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
	// Todo: check sender IP!!!

	if (message.getAddressPattern() == "/node")
	{
		OSCArgument* arg = message.begin();
		String argIp = arg->getString();
		arg++;
		String argName = arg->getString();

		Polytempo_NetworkSupervisor::getInstance()->addPeer(argIp, argName);
	}

#ifdef POLYTEMPO_NETWORK
	else if (message.getAddressPattern() == "/open")
	{
		DBG("osc: open");
		// "Open Score": only PolytempoNetwork

		Polytempo_NetworkApplication* const app = dynamic_cast<Polytempo_NetworkApplication*>(JUCEApplication::getInstance());
		OSCArgument* arg = message.begin();
		if (arg->isString())
			app->openScoreFilePath(arg->getString());
	}
#endif
	else
	{
		Array<var> *messages = new Array<var>();

		String address = message.getAddressPattern().toString();
		DBG("osc: " << address);

		OSCArgument* arg = message.begin();
		while (arg != message.end())
		{
			if ((*arg).isString())
				messages->add(var(String((arg)->getString())));
			else if ((*arg).isInt32())
				messages->add(var((int32)(arg)->getInt32()));
			else if ((*arg).isFloat32())
				messages->add(var((float)(arg)->getFloat32()));
			// Todo: Double not supported!!!
			else if ((*arg).isBlob())
				DBG("<blob>");
			else
				DBG("<unknown>");

			arg++;
		}

		Polytempo_Event *event = Polytempo_Event::makeEvent(address.toRawUTF8(), *messages);
		Polytempo_Scheduler::getInstance()->handleEvent(event, event->getTime());
	}
}
