#pragma once

#include "Polytempo_EventObserver.h"

class Polytempo_EventScheduler : public Thread
{
public:
    Polytempo_EventScheduler();
    ~Polytempo_EventScheduler();

    juce_DeclareSingleton(Polytempo_EventScheduler, false)

    /* schedule events
     --------------------------------------- */

    void scheduleScoreEvent(Polytempo_Event* event);
    void deletePendingEvents();

    void scheduleEvent(Polytempo_Event* event);
    void executeEvent(Polytempo_Event* event);

    /* thread
     --------------------------------------- */
    void run();

    /* notify event observers
     --------------------------------------- */
    void registerObserver(Polytempo_EventObserver* obs);
    void removeObserver(Polytempo_EventObserver* obs);
private:
    void notify(Polytempo_Event* event);

    Array<class Polytempo_EventObserver *> observers;
    Array<Polytempo_Event*, CriticalSection> scheduledScoreEvents;
    Array<Polytempo_Event*, CriticalSection> scheduledEvents;
    bool deleteEvents = false;
};
