#include "Polytempo_LibMain.h"



#include "Polytempo_LibEventHandler.h"
#include "../Network/Polytempo_NetworkSupervisor.h"
#include "../Network/Polytempo_OSCListener.h"
#include "../Network/Polytempo_TimeProvider.h"

Polytempo_LibMain::Ptr Polytempo_LibMain::current_;

Polytempo_LibMain::Polytempo_LibMain()
{
    // initialize event handler
    Polytempo_LibEventHandler::getInstance();
    isInit = false;
}

int Polytempo_LibMain::initialize(int port, bool masterFlag)
{
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
    Polytempo_InterprocessCommunication::getInstance()->distributeEvent(e, destinationNamePattern);
    delete e;

    return true;
}

Polytempo_LibMain::~Polytempo_LibMain()
{
    Polytempo_NetworkSupervisor::deleteInstance();
    Polytempo_TimeProvider::deleteInstance();
    Polytempo_InterprocessCommunication::deleteInstance();
    Polytempo_LibEventHandler::deleteInstance();
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