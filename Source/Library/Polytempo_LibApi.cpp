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

EXPORT_VOID polytempo_registerEventCallback(PolytempoEventCallbackHandler* pHandler, PolytempoEventCallbackOptions options)
{
    Polytempo_LibMain::current()->registerEventCallback(pHandler, options);
}

EXPORT_VOID polytempo_registerDetailedEventCallback(PolytempoDetailedEventCallbackHandler* pHandler, PolytempoEventCallbackOptions options)
{
    Polytempo_LibMain::current()->registerDetailedEventCallback(pHandler, options);
}

EXPORT_VOID polytempo_registerTickCallback(PolytempoTickCallbackHandler* pHandler, PolytempoTickCallbackOptions options)
{
    Polytempo_LibMain::current()->registerTickCallback(pHandler, options);
}

EXPORT_VOID polytempo_registerStateCallback(PolytempoStateCallbackHandler* pHandler, PolytempoStateCallbackOptions options)
{
    Polytempo_LibMain::current()->registerStateCallback(pHandler, options);
}

EXPORT_VOID polytempo_unregisterEventCallback(PolytempoEventCallbackHandler* pHandler)
{
    Polytempo_LibMain::current()->unregisterEventCallback(pHandler);
}

EXPORT_VOID polytempo_unregisterDetailedEventCallback(PolytempoDetailedEventCallbackHandler* pHandler)
{
    Polytempo_LibMain::current()->unregisterDetailedEventCallback(pHandler);
}

EXPORT_VOID polytempo_unregisterTickCallback(PolytempoTickCallbackHandler* pHandler)
{
    Polytempo_LibMain::current()->unregisterTickCallback(pHandler);
}

EXPORT_VOID polytempo_unregisterStateCallback(PolytempoStateCallbackHandler* pHandler)
{
    Polytempo_LibMain::current()->unregisterStateCallback(pHandler);
}


EXPORT_VOID polytempo_release()
{
    Polytempo_LibMain::release();
}
