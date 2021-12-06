#include "Polytempo_LibMain.h"

#include "../Network/Polytempo_NetworkSupervisor.h"
#include "../Network/Polytempo_OSCListener.h"
#include "../Network/Polytempo_TimeProvider.h"
#include "../Network/Polytempo_PortDefinition.h"
#include "../Scheduler/Polytempo_EventDispatcher.h"
#include "../Scheduler/Polytempo_EventScheduler.h"
#include "../Scheduler/Polytempo_ScoreSchedulerEngine.h"

Polytempo_LibMain::Ptr Polytempo_LibMain::current_;

Polytempo_LibMain::Polytempo_LibMain() : isInit(false), pEventCallback(nullptr), pTickCallback(nullptr), pStateCallback(nullptr)
{
    MessageManager::getInstance();
}


Polytempo_LibMain::~Polytempo_LibMain()
{
    oscListener = nullptr;
    Polytempo_NetworkSupervisor::deleteInstance();
    Polytempo_ScoreScheduler::deleteInstance();
    Polytempo_EventScheduler::deleteInstance();
    Polytempo_TimeProvider::deleteInstance();
    Polytempo_EventDispatcher::deleteInstance();
    Polytempo_InterprocessCommunication::deleteInstance();
}

int Polytempo_LibMain::initialize(int port, bool masterFlag)
{
    Polytempo_ScoreScheduler::getInstance()->setEngine(new Polytempo_NetworkEngine());
    Polytempo_EventScheduler::getInstance()->startThread(5); // priority between 0 and 10

    oscListener.reset(new Polytempo_OSCListener(port));
    Polytempo_NetworkSupervisor::getInstance()->createSender(Polytempo_PortDefinition::AdvertisePortCount, Polytempo_PortDefinition::AdvertisePorts);
    Polytempo_TimeProvider::getInstance()->registerUserInterface(this);
    bool ok = Polytempo_TimeProvider::getInstance()->toggleMaster(masterFlag);
    isInit = ok;

    return ok ? 0 : -1;
}

int Polytempo_LibMain::toggleMaster(bool masterFlag)
{
    bool ok = Polytempo_TimeProvider::getInstance()->toggleMaster(masterFlag);
    return ok ? 0 : -1;
}

int Polytempo_LibMain::sendEvent(std::string command, std::string payload, std::string destinationNamePattern)
{
    Polytempo_Event* e = Polytempo_Event::makeEvent(command);
    e->setType(command);

    // parse payload
    String payloadString(payload);
    auto tokens = StringArray::fromTokens(payloadString, " ", "\"");

    if(tokens.size() == 1)
    {
        e->setValue(tokens[0]);
    }
    else {
        for (int i = 0; i < tokens.size() / 2; i++)
        {
            e->setProperty(tokens[i * 2], tokens[i * 2 + 1]);
        }
    }

    if (e->getType() == eventType_Start 
        || e->getType() == eventType_Stop
        || e->getType() == eventType_Pause
        || e->getType() == eventType_GotoTime 
        || e->getType() == eventType_TempoFactor)
    {
        Polytempo_EventDispatcher::getInstance()->broadcastEvent(e);
    }
    else
    {
        if (e->getType() == eventType_Settings && (destinationNamePattern.empty() || destinationNamePattern == "*"))
        {
            Polytempo_EventScheduler::getInstance()->scheduleEvent(e);
        }
        else
        {
            Polytempo_InterprocessCommunication::getInstance()->distributeEvent(e, destinationNamePattern);
            delete e;
        }
    }
    
    return 0;
}

int Polytempo_LibMain::sendEvent(std::string fullEventString)
{
    String str(fullEventString);
    String commandAndPayload;
    String command, payload;
    String addressPattern = "*";
    if(str.matchesWildcard("/*/*", true))
    {
        addressPattern = str.upToLastOccurrenceOf("/", false, true).trimCharactersAtStart("/");
        commandAndPayload = str.fromLastOccurrenceOf("/", false, true);
    }
    else
    {
        commandAndPayload = str.trimCharactersAtStart("/");
    }

    if(commandAndPayload.contains(" "))
    {
        command = commandAndPayload.upToFirstOccurrenceOf(" ", false, true);
        payload = commandAndPayload.fromFirstOccurrenceOf(" ", false, true);
    }
    else
    {
        command = commandAndPayload;
    }

    sendEvent(command.toStdString(), payload.toStdString(), addressPattern.toStdString());

    return 0;
}

void Polytempo_LibMain::registerEventCallback(EventCallbackHandler* pHandler)
{
    pEventCallback = pHandler;
}

void Polytempo_LibMain::registerTickCallback(TickCallbackHandler* pHandler)
{
    pTickCallback = pHandler;
}

void Polytempo_LibMain::registerStateCallback(StateCallbackHandler* pHandler)
{
    pStateCallback = pHandler;
}

Polytempo_LibMain::Ptr Polytempo_LibMain::current()
{
    if (!current_)
    {
        current_ = new Polytempo_LibMain;
    }
    return current_;
}

void Polytempo_LibMain::release()
{
    current_ = nullptr;
}

void Polytempo_LibMain::eventNotification(Polytempo_Event* event)
{
    if (event->getType() == eventType_Tick)
    {
        if (pTickCallback != nullptr)
        {
            //MessageManager::callAsync([this, event]{ pTickCallback->processTick(event->getValue()); });
            pTickCallback->processTick(event->getValue());
        }
    }
    else
    {
        if (pEventCallback != nullptr)
        {
            String str = event->getTypeString() + " ";
            const NamedValueSet* props = event->getProperties();
            if(!props->isEmpty())
            {
                for (auto& e : *props)
                    str.append(e.name.toString() + " " + e.value.toString() + " ", 100);
            }

            //MessageManager::callAsync([this, str] { pEventCallback->processEvent(str.trimCharactersAtEnd(" ").toStdString()); });
            pEventCallback->processEvent(str.trimCharactersAtEnd(" ").toStdString());
        }
    }
}

void Polytempo_LibMain::showInfoMessage(int messageType, String message)
{
    if (pStateCallback != nullptr)
    {
        // build connected peers list
        OwnedArray<Polytempo_PeerInfo> peers;
        std::vector<std::string> peerString;
        
        if(Polytempo_TimeProvider::getInstance()->isMaster())
        {
            Polytempo_InterprocessCommunication::getInstance()->getClientsInfo(&peers);
        }
        else
        {
            auto info = Polytempo_InterprocessCommunication::getInstance()->getMasterInfo();
            if(info != nullptr)
            {
                peers.add(info);
            }
        }
        
        for (Polytempo_PeerInfo* peer : peers)
        {
            String name = peer->scoreName + " (" + (peer->peerName.isNotEmpty() ? peer->peerName : "-") + ")";
            peerString.push_back(name.toStdString());
        }
        
        pStateCallback->processState(messageType, message.toStdString(), peerString);
    }
}
