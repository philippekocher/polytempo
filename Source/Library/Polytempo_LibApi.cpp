#include "Polytempo_LibApi.h"
#include "Polytempo_LibMain.h"

EXPORT_INT polytempo_initialize(int port, bool masterFlag)
{
    return Polytempo_LibMain::current()->initialize(port, masterFlag);
}

EXPORT_INT polytempo_toggleMaster(bool masterFlag)
{
    return Polytempo_LibMain::current()->toggleMaster(masterFlag);
}

EXPORT_INT polytempo_sendEvent(std::string fullEventString)
{
    return Polytempo_LibMain::current()->sendEvent(fullEventString);
}

EXPORT_VOID polytempo_registerEventCallback(EventCallbackHandler* pHandler)
{
    Polytempo_LibMain::current()->registerEventCallback(pHandler);
}

EXPORT_VOID polytempo_registerTickCallback(TickCallbackHandler* pHandler)
{
    Polytempo_LibMain::current()->registerTickCallback(pHandler);
}

EXPORT_VOID polytempo_registerStateCallback(StateCallbackHandler* pHandler)
{
    Polytempo_LibMain::current()->registerStateCallback(pHandler);
}

EXPORT_VOID polytempo_release()
{
    Polytempo_LibMain::release();
}
