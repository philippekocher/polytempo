#pragma once

#include "Polytempo_LibCallbackHandler.h"
#include <string>

#ifndef TIME_SYNC_OK
#define TIME_SYNC_OK 0
#endif
#ifndef TIME_SYNC_NOK
#define TIME_SYNC_NOK -1
#endif

#if JUCE_WINDOWS
#define EXPORT_VOID    __declspec(dllexport) void  __stdcall
#define EXPORT_INT     __declspec(dllexport) int  __stdcall
#define EXPORT_DOUBLE  __declspec(dllexport) double  __stdcall
#else
#define EXPORT_VOID    void
#define EXPORT_INT     int
#define EXPORT_DOUBLE  double
#endif

extern "C"
{
    EXPORT_INT polytempo_initialize(int port, bool masterFlag, std::string instanceName);
    EXPORT_INT polytempo_isInitialized();
    EXPORT_INT polytempo_toggleMaster(bool masterFlag);
    EXPORT_INT polytempo_sendSimpleEvent(std::string fullEventString);
    EXPORT_INT polytempo_sendEvent(PolytempoEventDto dto);

    EXPORT_VOID polytempo_registerSimpleEventCallback(PolytempoSimpleEventCallbackHandler* pHandler, PolytempoEventCallbackOptions options);
    EXPORT_VOID polytempo_registerEventCallback(PolytempoEventCallbackHandler* pHandler, PolytempoEventCallbackOptions options);
    EXPORT_VOID polytempo_registerTickCallback(PolytempoTickCallbackHandler* pHandler, PolytempoTickCallbackOptions options);
    EXPORT_VOID polytempo_registerStateCallback(PolytempoStateCallbackHandler* pHandler, PolytempoStateCallbackOptions options);

    EXPORT_VOID polytempo_unregisterSimpleEventCallback(PolytempoSimpleEventCallbackHandler* pHandler);
    EXPORT_VOID polytempo_unregisterEventCallback(PolytempoEventCallbackHandler* pHandler);
    EXPORT_VOID polytempo_unregisterTickCallback(PolytempoTickCallbackHandler* pHandler);
    EXPORT_VOID polytempo_unregisterStateCallback(PolytempoStateCallbackHandler* pHandler);

    EXPORT_VOID polytempo_release();
}
