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

EXPORT_VOID polytempo_setClientName(std::string name)
{
    Polytempo_LibMain::current()->setClientName(name);
}

EXPORT_INT polytempo_getTime(uint32_t* pTime)
{
    return Polytempo_LibMain::current()->getTime(pTime);
}

EXPORT_VOID polytempo_release()
{
    Polytempo_LibMain::release();
}
