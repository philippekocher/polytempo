#include "Polytempo_LibApi.h"
#include "Polytempo_LibMain.h"

EXPORT_INT polytempo_initialize(int port, bool masterFlag, std::string instanceName)
{
    return Polytempo_LibMain::current()->initialize(port, masterFlag, instanceName);
}

EXPORT_INT polytempo_isInitialized()
{
    return Polytempo_LibMain::current()->isInitialized();
}

EXPORT_INT polytempo_toggleMaster(bool masterFlag)
{
    return Polytempo_LibMain::current()->toggleMaster(masterFlag);
}

EXPORT_INT polytempo_sendEvent(std::string fullEventString)
{
    return Polytempo_LibMain::current()->sendEvent(fullEventString);
}

EXPORT_VOID polytempo_registerEventCallback(EventCallbackHandler* pHandler, EventCallbackOptions options)
{
    Polytempo_LibMain::current()->registerEventCallback(pHandler, options);
}

EXPORT_VOID polytempo_registerTickCallback(TickCallbackHandler* pHandler, TickCallbackOptions options)
{
    Polytempo_LibMain::current()->registerTickCallback(pHandler, options);
}

EXPORT_VOID polytempo_registerStateCallback(StateCallbackHandler* pHandler, StateCallbackOptions options)
{
    Polytempo_LibMain::current()->registerStateCallback(pHandler, options);
}

EXPORT_VOID polytempo_unregisterEventCallback(EventCallbackHandler* pHandler)
{
    Polytempo_LibMain::current()->unregisterEventCallback(pHandler);
}

EXPORT_VOID polytempo_unregisterTickCallback(TickCallbackHandler* pHandler)
{
    Polytempo_LibMain::current()->unregisterTickCallback(pHandler);
}

EXPORT_VOID polytempo_unregisterStateCallback(StateCallbackHandler* pHandler)
{
    Polytempo_LibMain::current()->unregisterStateCallback(pHandler);
}


EXPORT_VOID polytempo_release()
{
    Polytempo_LibMain::release();
}
