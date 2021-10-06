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
    EXPORT_INT polytempo_initialize(int port, bool masterFlag);
    EXPORT_INT polytempo_toggleMaster(bool masterFlag);
    EXPORT_INT polytempo_sendEvent(std::string fullEventString);
    EXPORT_VOID polytempo_registerEventCallback(EventCallbackHandler* pHandler);
    EXPORT_VOID polytempo_registerTickCallback(TickCallbackHandler* pHandler);
    EXPORT_VOID polytempo_registerStateCallback(StateCallbackHandler* pHandler);
    EXPORT_VOID polytempo_release();
}
