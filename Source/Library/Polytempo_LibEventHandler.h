#pragma once
#include "../Scheduler/Polytempo_Event.h"
#include "../Scheduler/Polytempo_EventObserver.h"

class Polytempo_LibEventHandler
{
public:
    juce_DeclareSingleton(Polytempo_LibEventHandler, false)
    Polytempo_LibEventHandler();
    virtual ~Polytempo_LibEventHandler();
    void handleEvent(Polytempo_Event* e);
    void registerObserver(Polytempo_EventObserver* observer);
    void removeObserver(Polytempo_EventObserver* observer);
    
private:
    Array<class Polytempo_EventObserver*> observers;
};
