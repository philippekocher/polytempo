#pragma once
#include "../Scheduler/Polytempo_Event.h"
#include "../Scheduler/Polytempo_EventObserver.h"

class Polytempo_LibEventHandler
{
public:
    void handleEvent(Polytempo_Event* e);
    void registerObserver(Polytempo_EventObserver* observer);
    void removeObserver(Polytempo_EventObserver* observer);

    juce_DeclareSingleton(Polytempo_LibEventHandler, false)

private:
    Array<class Polytempo_EventObserver*> observers;
};
