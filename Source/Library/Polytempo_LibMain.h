#pragma once
#include <JuceHeader.h>


#include "../Network/Polytempo_OSCListener.h"
#include "../Scheduler/Polytempo_LibEventObserver.h"
#include "Polytempo_LibCallbackHandler.h"
#include "../Network/Polytempo_TimeSyncInfoInterface.h"

#define TIME_SYNC_OK 0
#define TIME_SYNC_NOK -1

class Polytempo_LibMain : public ReferenceCountedObject, Polytempo_EventObserver, public Polytempo_TimeSyncInfoInterface
{
private:
    Polytempo_LibMain();

public:
    virtual ~Polytempo_LibMain();

    int initialize(int port, bool masterFlag, std::string instanceName);
    bool isInitialized();
    int toggleMaster(bool masterFlag);
    int sendEvent(std::string command, std::string payload, std::string destinationNamePattern = "*");
    int sendEvent(std::string fullEventString);
    void registerEventCallback(PolytempoEventCallbackHandler* pHandler, PolytempoEventCallbackOptions options);
    void registerDetailedEventCallback(PolytempoDetailedEventCallbackHandler* pHandler, PolytempoEventCallbackOptions options);
    void registerTickCallback(PolytempoTickCallbackHandler* pHandler, PolytempoTickCallbackOptions options);
    void registerStateCallback(PolytempoStateCallbackHandler* pHandler, PolytempoStateCallbackOptions options);
    void unregisterEventCallback(PolytempoEventCallbackHandler* pHandler);
    void unregisterDetailedEventCallback(PolytempoDetailedEventCallbackHandler* pHandler);
    void unregisterTickCallback(PolytempoTickCallbackHandler* pHandler);
    void unregisterStateCallback(PolytempoStateCallbackHandler* pHandler);
    
    typedef ReferenceCountedObjectPtr<Polytempo_LibMain> Ptr;
    static Ptr current();
    static void release();

    
private:
    void eventNotification(Polytempo_Event* event) override;
    void showInfoMessage(int messageType, String message) override;

private:
    bool isInit;
    HashMap<PolytempoEventCallbackHandler*, PolytempoEventCallbackOptions> eventCallbacks;
    HashMap<PolytempoDetailedEventCallbackHandler*, PolytempoEventCallbackOptions> detailedEventCallbacks;
    HashMap<PolytempoTickCallbackHandler*, PolytempoTickCallbackOptions> tickCallbacks;
    HashMap<PolytempoStateCallbackHandler*, PolytempoStateCallbackOptions> stateCallbacks;
    std::string lastStatusInfo;

    static Ptr current_;
    std::unique_ptr<Polytempo_OSCListener> oscListener;
};

