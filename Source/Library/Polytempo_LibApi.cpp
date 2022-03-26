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

EXPORT_INT polytempo_sendSimpleEvent(std::string fullEventString)
{
    return Polytempo_LibMain::current()->sendSimpleEvent(fullEventString);
}

EXPORT_INT polytempo_sendEvent(PolytempoEventDto dto)
{
    return Polytempo_LibMain::current()->sendEvent(dto);
}

EXPORT_VOID polytempo_registerSimpleEventCallback(PolytempoSimpleEventCallbackHandler* pHandler, PolytempoEventCallbackOptions options)
{
    Polytempo_LibMain::current()->registerSimpleEventCallback(pHandler, options);
}

EXPORT_VOID polytempo_registerEventCallback(PolytempoEventCallbackHandler* pHandler, PolytempoEventCallbackOptions options)
{
    Polytempo_LibMain::current()->registerEventCallback(pHandler, options);
}

EXPORT_VOID polytempo_registerTickCallback(PolytempoTickCallbackHandler* pHandler, PolytempoTickCallbackOptions options)
{
    Polytempo_LibMain::current()->registerTickCallback(pHandler, options);
}

EXPORT_VOID polytempo_registerStateCallback(PolytempoStateCallbackHandler* pHandler, PolytempoStateCallbackOptions options)
{
    Polytempo_LibMain::current()->registerStateCallback(pHandler, options);
}

EXPORT_VOID polytempo_unregisterSimpleEventCallback(PolytempoSimpleEventCallbackHandler* pHandler)
{
    Polytempo_LibMain::current()->unregisterSimpleEventCallback(pHandler);
}

EXPORT_VOID polytempo_unregisterEventCallback(PolytempoEventCallbackHandler* pHandler)
{
    Polytempo_LibMain::current()->unregisterEventCallback(pHandler);
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
