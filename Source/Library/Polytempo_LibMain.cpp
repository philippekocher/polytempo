#include "Polytempo_LibMain.h"

#include "../Network/Polytempo_NetworkSupervisor.h"
#include "../Network/Polytempo_OSCListener.h"
#include "../Network/Polytempo_TimeProvider.h"
#include "../Network/Polytempo_PortDefinition.h"
#include "../Scheduler/Polytempo_EventDispatcher.h"
#include "../Scheduler/Polytempo_EventScheduler.h"
#include "../Scheduler/Polytempo_ScoreSchedulerEngine.h"

Polytempo_LibMain::Ptr Polytempo_LibMain::current_;

Polytempo_LibMain::Polytempo_LibMain() : isInit(false)
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

int Polytempo_LibMain::initialize(int port, bool masterFlag, std::string instanceName)
{
    Polytempo_ScoreScheduler::getInstance()->setEngine(new Polytempo_NetworkEngine());
    Polytempo_EventScheduler::getInstance()->startThread(5); // priority between 0 and 10

    oscListener.reset(new Polytempo_OSCListener(port));
    Polytempo_NetworkSupervisor::getInstance()->createSender(Polytempo_PortDefinition::AdvertisePortCount, Polytempo_PortDefinition::AdvertisePorts);
    Polytempo_NetworkSupervisor::getInstance()->setManualPeerName(instanceName);
    Polytempo_TimeProvider::getInstance()->registerUserInterface(this);
    bool ok = Polytempo_TimeProvider::getInstance()->toggleMaster(masterFlag);
    isInit = ok;

    return ok ? 0 : -1;
}

bool Polytempo_LibMain::isInitialized()
{
    return current_ != nullptr && isInit;
}

int Polytempo_LibMain::toggleMaster(bool masterFlag)
{
    bool wasMaster = Polytempo_TimeProvider::getInstance()->isMaster();
    bool ok = Polytempo_TimeProvider::getInstance()->toggleMaster(masterFlag);
    bool isMaster = Polytempo_TimeProvider::getInstance()->isMaster();
    if(isMaster != wasMaster)
    {
        for (HashMap<PolytempoStateCallbackHandler*, PolytempoStateCallbackOptions>::Iterator i (stateCallbacks); i.next();) {
            i.getKey()->processMasterChanged(isMaster);
        }
    }
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

    if(Polytempo_TimeProvider::getInstance()->isMaster() && !destinationNamePattern.empty())
    {
        Polytempo_InterprocessCommunication::getInstance()->distributeEvent(e, destinationNamePattern);
    }
    else if(destinationNamePattern.empty() || Polytempo_NetworkSupervisor::getInstance()->getScoreName().matchesWildcard(destinationNamePattern, true))
    {
        e->setSyncTime(Polytempo_TimeProvider::getInstance()->getDelaySafeTimestamp());
        Polytempo_EventScheduler::getInstance()->scheduleEvent(e);
    }
    
    return 0;
}

int Polytempo_LibMain::sendEvent(std::string fullEventString)
{
    String str(fullEventString);
    String commandAndPayload;
    String command, payload;
    String addressPattern = "";
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

void Polytempo_LibMain::registerEventCallback(PolytempoEventCallbackHandler* pHandler, PolytempoEventCallbackOptions options)
{
    eventCallbacks.set(pHandler, options);
}

void Polytempo_LibMain::registerDetailedEventCallback(PolytempoDetailedEventCallbackHandler* pHandler, PolytempoEventCallbackOptions options)
{
    detailedEventCallbacks.set(pHandler, options);
}

void Polytempo_LibMain::registerTickCallback(PolytempoTickCallbackHandler* pHandler, PolytempoTickCallbackOptions options)
{
    tickCallbacks.set(pHandler, options);
    pHandler->processTick(0.0);
}

void Polytempo_LibMain::registerStateCallback(PolytempoStateCallbackHandler* pHandler, PolytempoStateCallbackOptions options)
{
    stateCallbacks.set(pHandler, options);
    pHandler->processMasterChanged(Polytempo_TimeProvider::getInstance()->isMaster());
    pHandler->processState(0, "Initializing", std::vector<std::string>({"-"}));
}

void Polytempo_LibMain::unregisterEventCallback(PolytempoEventCallbackHandler *pHandler)
{
    eventCallbacks.remove(pHandler);
}

void Polytempo_LibMain::unregisterDetailedEventCallback(PolytempoDetailedEventCallbackHandler *pHandler)
{
    detailedEventCallbacks.remove(pHandler);
}

void Polytempo_LibMain::unregisterTickCallback(PolytempoTickCallbackHandler *pHandler)
{
    tickCallbacks.remove(pHandler);
}

void Polytempo_LibMain::unregisterStateCallback(PolytempoStateCallbackHandler *pHandler)
{
    stateCallbacks.remove(pHandler);
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
    if(current_ != nullptr
       && current_->eventCallbacks.size() == 0
       && current_->stateCallbacks.size() == 0
       && current_->tickCallbacks.size() == 0)
    current_ = nullptr;
}

void Polytempo_LibMain::eventNotification(Polytempo_Event* event)
{
    if (event->getType() == eventType_Tick)
    {
        for (HashMap<PolytempoTickCallbackHandler*, PolytempoTickCallbackOptions>::Iterator i (tickCallbacks); i.next();) {
            i.getKey()->processTick(event->getValue());
        }
    }
    else
    {
        for (HashMap<PolytempoEventCallbackHandler*, PolytempoEventCallbackOptions>::Iterator i (eventCallbacks); i.next();) {
            String str = event->getTypeString() + " ";
            const NamedValueSet* props = event->getProperties();
            if(!props->isEmpty())
            {
                for (auto& e : *props) {
                    if(i.getValue().ignoreTimeTag && e.name == Identifier("timeTag"))
                        continue;
                    str.append(e.name.toString() + " " + e.value.toString() + " ", 100);
                }
            }

            i.getKey()->processEvent(str.trimCharactersAtEnd(" ").toStdString());
        }
        
        // detailed event callbacks
        for (HashMap<PolytempoDetailedEventCallbackHandler*, PolytempoEventCallbackOptions>::Iterator i (detailedEventCallbacks); i.next();) {
            
            PolytempoDetailedEventCallbackObject o;
            o.command = event->getTypeString().toStdString();
            const NamedValueSet* props = event->getProperties();
            if(!props->isEmpty())
            {
                for (auto& e : *props) {
                    if(i.getValue().ignoreTimeTag && e.name == Identifier("timeTag"))
                        continue;
                    PolytempoEventArgument a;
                    a.name = e.name.toString().toStdString();
                    
                    if(e.value.isInt() || e.value.isBool())
                        a.eValueType = PolytempoEventArgument::EValueType::Value_Int;
                    else if(e.value.isDouble())
                        a.eValueType = PolytempoEventArgument::EValueType::Value_Double;
                    else if(e.value.isInt64())
                        a.eValueType = PolytempoEventArgument::EValueType::Value_Int64;
                    else // everything else is considered as string
                        a.eValueType = PolytempoEventArgument::EValueType::Value_String;
                    a.valueString = e.value.toString().toStdString();
                    o.arguments.push_back(a);
                }
            }

            i.getKey()->processEvent(o);
        }
    }
}

void Polytempo_LibMain::showInfoMessage(int messageType, String message)
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
    
    std::string currentFullString = message.toStdString();
    for(auto s : peerString) {
        currentFullString += (" " + s);
    }
    
    for (HashMap<PolytempoStateCallbackHandler*, PolytempoStateCallbackOptions>::Iterator i (stateCallbacks); i.next();) {
        if(i.getValue().callOnChangeOnly && currentFullString == lastStatusInfo)
            continue;
        
        i.getKey()->processState(messageType, message.toStdString(), peerString);
    }
    
    lastStatusInfo = currentFullString;
}
