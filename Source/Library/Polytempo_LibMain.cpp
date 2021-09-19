#include "Polytempo_LibMain.h"

#include "../Network/Polytempo_NetworkSupervisor.h"
#include "../Network/Polytempo_OSCListener.h"
#include "../Network/Polytempo_TimeProvider.h"
#include "../Scheduler/Polytempo_EventScheduler.h"

Polytempo_LibMain::Ptr Polytempo_LibMain::current_;

Polytempo_LibMain::Polytempo_LibMain() : isInit(false), pEventCallback(nullptr)
{

}


Polytempo_LibMain::~Polytempo_LibMain()
{
    Polytempo_NetworkSupervisor::deleteInstance();
    Polytempo_EventScheduler::deleteInstance();
    Polytempo_TimeProvider::deleteInstance();
    Polytempo_InterprocessCommunication::deleteInstance();
}

int Polytempo_LibMain::initialize(int port, bool masterFlag)
{
    Polytempo_EventScheduler::getInstance()->startThread(5); // priority between 0 and 10

    oscListener.reset(new Polytempo_OSCListener(port));
    Polytempo_NetworkSupervisor::getInstance()->createSender(port);
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

    Polytempo_InterprocessCommunication::getInstance()->distributeEvent(e, destinationNamePattern);
    delete e;

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

void Polytempo_LibMain::setClientName(std::string name)
{
    Polytempo_NetworkSupervisor::getInstance()->setScoreName(String(name));
}

int Polytempo_LibMain::getTime(uint32_t* pTime)
{
    return Polytempo_TimeProvider::getInstance()->getSyncTime(pTime) ? TIME_SYNC_OK : TIME_SYNC_NOK;
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

void Polytempo_LibMain::actionListenerCallback(const String& message)
{
    
}

void Polytempo_LibMain::eventNotification(Polytempo_Event* event)
{
    if (pEventCallback != nullptr)
    {
        // TODO serialize event
        pEventCallback->processEvent(event->getTypeString().toStdString());
    }
}
