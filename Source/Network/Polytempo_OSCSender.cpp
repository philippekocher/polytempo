#include "Polytempo_OSCSender.h"
#include "Polytempo_NetworkSupervisor.h"

Polytempo_OSCSender::Polytempo_OSCSender()
{
    oscSender.connect("localhost", 0);
}

Polytempo_OSCSender::~Polytempo_OSCSender()
{
    deleteAll();
}

void Polytempo_OSCSender::deleteAll()
{
    targetMap.clear();
}

void Polytempo_OSCSender::addSender(const juce::String &senderID, const juce::String &hostName, int port, bool ticks)
{
    Target target;
    target.hostName = hostName;
    target.port = port;
    target.ticks = ticks;

    targetMap.set(senderID, target);
}

void Polytempo_OSCSender::eventNotification(Polytempo_Event *event)
{
    if (event->getType() == eventType_AddSender)
    {
        addSender(event->getProperty("senderID"), event->getProperty("ip"), event->getProperty("port"), event->getProperty("tick"));
    }
    else if(event->getType() == eventType_DeleteAll)
        deleteAll();
    else if(event->getType() == eventType_Osc)
        sendOscEventAsMessage(event);
    else if(event->getType() == eventType_Tick   ||
            event->getType() == eventType_Start  ||
            event->getType() == eventType_Stop ||
            event->getType() == eventType_GotoTime)
        sendTick(event);
}

void Polytempo_OSCSender::sendOscEventAsMessage(Polytempo_Event *event)
{
    if (!event->hasProperty("address") || !event->getProperty("address").toString().startsWith("/"))
    {
        String msg = "Malformed OSC address: " + event->getProperty("address").toString();
        DBG(msg);
        Logger::writeToLog(msg);
        return;
    }

	OSCMessage oscMessage = OSCMessage(OSCAddressPattern(event->getProperty("address").toString()));
	var message = event->getProperty("message");
	Array < var > *messageArray;

	if (message.isArray())
	{
		messageArray = message.getArray();
	}
	else
	{
		messageArray = new Array < var >;
		messageArray->add(message);
	}

    for (var msg : *messageArray)
	{
		if (msg.isInt())          oscMessage.addInt32(int(msg));
		else if (msg.isDouble())  oscMessage.addFloat32(float(msg));
		else if (msg.isString())  oscMessage.addString(msg.toString());
	}

    if(targetMap.contains(event->getProperty("senderID")))
	{
        Target target = targetMap[event->getProperty("senderID")];
        oscSender.sendToIPAddress(target.hostName, target.port, oscMessage);
	}
}

void Polytempo_OSCSender::sendTick(Polytempo_Event *event)
{
    HashMap < String, Target >::Iterator it(targetMap);
    while(it.next())
    {
        Target target = it.getValue();
        if(target.ticks) sendEventAsMessage(event, target);
    }
}


void Polytempo_OSCSender::sendEventAsMessage(Polytempo_Event* event, Target target)
{
    OSCMessage oscMessage = OSCMessage(OSCAddressPattern(event->getOscAddressFromType()));
    for (var message : event->getOscMessageFromProperties())
    {
        if (message.isInt())          oscMessage.addInt32(int(message));
        else if (message.isDouble())  oscMessage.addFloat32(float(message));
        else if (message.isString())  oscMessage.addString(message.toString());
    }

    oscSender.sendToIPAddress(target.hostName, target.port, oscMessage);
}
