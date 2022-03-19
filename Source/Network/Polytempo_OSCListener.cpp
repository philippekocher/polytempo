#include "Polytempo_OSCListener.h"
#include "Polytempo_NetworkSupervisor.h"
#include "../Scheduler/Polytempo_EventScheduler.h"
#include "../Scheduler/Polytempo_ScoreScheduler.h"

#ifdef POLYTEMPO_NETWORK
#include "../Misc/Polytempo_Alerts.h"
#include "../Application/PolytempoNetwork/Polytempo_NetworkApplication.h"
#endif

#include "Polytempo_TimeProvider.h"

Polytempo_OSCListener::Polytempo_OSCListener(int port) : m_Port(port)
{
    oscReceiver.reset(new OSCReceiver());

    if (!oscReceiver->connect(m_Port))
#ifdef POLYTEMPO_NETWORK
        Polytempo_Alert::show("Error", "Can't bind to port: " + String(m_Port) + "\nProbably there is another socket already bound to this port");
#else
        DBG("Error initializing OSC listener"); // TODO!!!
#endif
    oscReceiver->addListener(this);
}

Polytempo_OSCListener::~Polytempo_OSCListener()
{
    oscReceiver = nullptr;
}

Polytempo_Event* Polytempo_OSCListener::oscToEvent(const OSCMessage& message, String addressPattern) const
{
    std::unique_ptr<Array<var>> messages = std::unique_ptr<Array<var>>{new Array<var>()};
    const OSCArgument* argumentIterator = message.begin();

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

    return Polytempo_Event::makeEvent(addressPattern, *messages);
}

void Polytempo_OSCListener::oscMessageReceived(const OSCMessage& message)
{
    String addressPattern = message.getAddressPattern().toString();
    String nodeNamePattern = "";
    String messagePattern;
    const OSCArgument* argumentIterator = message.begin();
    
    if(addressPattern.matchesWildcard("/*/*", true))
    {
        // parse pattern
        addressPattern = addressPattern.substring(1);
        nodeNamePattern = addressPattern.upToFirstOccurrenceOf("/", false, false);
        messagePattern = addressPattern.substring(nodeNamePattern.length());
    }
    else
    {
        messagePattern = addressPattern;
    }
    
    if (messagePattern == "/masteradvertise")
    {
        Uuid senderId = Uuid((argumentIterator++)->getString());
        if (senderId == Polytempo_NetworkSupervisor::getInstance()->getUniqueId())
            return;

        String argIp = (argumentIterator++)->getString();

        Polytempo_TimeProvider::getInstance()->setRemoteMasterPeer(argIp, senderId);
    }

#if defined(POLYTEMPO_NETWORK) || defined(POLYTEMPO_LIB)
    else if (!nodeNamePattern.isEmpty() && Polytempo_TimeProvider::getInstance()->isMaster())
    {
        
        Polytempo_InterprocessCommunication::getInstance()->distributeEvent(oscToEvent(message, messagePattern), nodeNamePattern);
    }
#endif
    else if (nodeNamePattern.isEmpty()
             || Polytempo_NetworkSupervisor::getInstance()->getScoreName().matchesWildcard(nodeNamePattern, true)
             || Polytempo_NetworkSupervisor::getInstance()->getPeerName().matchesWildcard(nodeNamePattern, true)
             )
    {
        Polytempo_Event* event = oscToEvent(message, messagePattern);

        if (event->getType() == eventType_None)
        {
            DBG("--unknown");
            delete event;
            return;
        }
        
        // calculate syncTime
        uint32 syncTime;

        if (event->hasProperty(eventPropertyString_TimeTag))
            syncTime = uint32(int32(event->getProperty(eventPropertyString_TimeTag)));
        else
        {
            Polytempo_TimeProvider::getInstance()->getSyncTime(&syncTime);
        }

        if (event->hasProperty(eventPropertyString_Time))
        {
            Polytempo_ScoreScheduler* scoreScheduler = Polytempo_ScoreScheduler::getInstance();

            if (!scoreScheduler->isRunning()) return; // ignore an event that has a "time"
            // when the score scheduler isn't running

            syncTime += uint32(event->getTime() - scoreScheduler->getScoreTime());
        }

        if (event->hasProperty(eventPropertyString_Defer))
            syncTime += uint32(float(event->getProperty(eventPropertyString_Defer)) * 1000.0f);

        event->setSyncTime(syncTime);

        Polytempo_EventScheduler::getInstance()->scheduleEvent(event);
    }
}
