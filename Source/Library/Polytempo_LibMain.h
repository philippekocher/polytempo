#pragma once
#include <JuceHeader.h>


#include "../Network/Polytempo_OSCListener.h"
#include "../Scheduler/Polytempo_EventObserver.h"
#include "../Scheduler/Polytempo_LibEventObserver.h"

class Polytempo_LibMain : public ReferenceCountedObject, ActionListener
{
private:
    Polytempo_LibMain();

public:
    virtual ~Polytempo_LibMain();

    int initialize(int port, bool masterFlag);
    int toggleMaster(bool masterFlag);
    int sendEvent(std::string command, std::string payload, std::string destinationNamePattern = "*");
    int sendEvent(std::string fullEventString);
    void registerEventCallback(void(*event_callback)(std::string));

    typedef ReferenceCountedObjectPtr<Polytempo_LibMain> Ptr;
    static Ptr current();
    static void release();

private:
    void actionListenerCallback(const String& message) override;
    void (*eventCallback)(std::string);
    static Ptr current_;
    std::unique_ptr<Polytempo_OSCListener> oscListener;
    std::unique_ptr<Polytempo_LibEventObserver> eventObserver;
    bool isInit;
};
