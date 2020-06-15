#include "Polytempo_EventScheduler.h"
#include "../Network/Polytempo_TimeProvider.h"

Polytempo_EventScheduler::Polytempo_EventScheduler() : Thread("Polytempo_EventScheduler_Thread")
{
}

Polytempo_EventScheduler::~Polytempo_EventScheduler()
{
    // allow the thread 2 seconds to stop cleanly - should be plenty of time.
    stopThread(2000);

    clearSingletonInstance();
}

juce_ImplementSingleton(Polytempo_EventScheduler);

// ----------------------------------------------------
#pragma mark -
#pragma mark schedule events

void Polytempo_EventScheduler::scheduleScoreEvent(Polytempo_Event* event)
{
    if (event == nullptr) return;
    if (event->getType() == eventType_None) return;

    uint32 currentSyncTime;
    Polytempo_TimeProvider::getInstance()->getSyncTime(&currentSyncTime);
    if (event->getSyncTime() <= currentSyncTime)
    {
        notify(event);
        if (!event->isOwned()) delete event;
    }
    else
    {
        scheduledScoreEvents.add(event);
    }
}

void Polytempo_EventScheduler::deletePendingEvents()
{
    deleteEvents = true;
}

void Polytempo_EventScheduler::scheduleEvent(Polytempo_Event* event)
{
    if (event == nullptr) return;
    if (event->getType() == eventType_None) return;

    uint32 currentSyncTime;
    Polytempo_TimeProvider::getInstance()->getSyncTime(&currentSyncTime);
    if (event->getSyncTime() <= currentSyncTime)
    {
        notify(event);

        // logging for late start messages
        if (event->getType() == eventType_Start)
        {
            Logger::writeToLog("Late start command received! Delay: " + String(currentSyncTime - event->getSyncTime()) + "; Current MRT: " + String(Polytempo_TimeProvider::getInstance()->getMRT()));
        }

        if (!event->isOwned()) delete event;
    }
    else
    {
        scheduledEvents.add(event);
    }
}

void Polytempo_EventScheduler::executeEvent(Polytempo_Event* event)
{
    // execute the event immediately
    notify(event);
}

// ----------------------------------------------------
#pragma mark -
#pragma mark thread

void Polytempo_EventScheduler::run()
{
    int interval = 5;
    uint32 currentSyncTime;

    while (!threadShouldExit())
    {
        // get current sync time
        Polytempo_TimeProvider::getInstance()->getSyncTime(&currentSyncTime);
        if (deleteEvents)
        {
            scheduledScoreEvents.clear();
            scheduledEvents.clear();
            deleteEvents = false;
        }

        for (int i = 0; i < scheduledScoreEvents.size();)
        {
            Polytempo_Event* event = scheduledScoreEvents[i];

            if (event->getSyncTime() <= currentSyncTime)
            {
                notify(event);
                if (!event->isOwned()) delete event;
                scheduledScoreEvents.remove(i);
            }
            else ++i;
        }
        for (int j = 0; j < scheduledEvents.size();)
        {
            Polytempo_Event* event = scheduledEvents[j];

            if (event->getSyncTime() <= currentSyncTime)
            {
                notify(event);
                if (!event->isOwned()) delete event;
                scheduledEvents.remove(j);
            }
            else ++j;
        }
        wait(interval);
    }
}

// ----------------------------------------------------
#pragma mark -
#pragma mark notify event observers

void Polytempo_EventScheduler::registerObserver(Polytempo_EventObserver* obs)
{
    observers.add(obs);
}

void Polytempo_EventScheduler::removeObserver(Polytempo_EventObserver* obs)
{
    observers.removeAllInstancesOf(obs);
}

void Polytempo_EventScheduler::notify(Polytempo_Event* event)
{
    // because this is a background thread, we mustn't do any UI work without
    // first grabbing a MessageManagerLock..
    const MessageManagerLock mml(Thread::getCurrentThread());

    if (! mml.lockWasGained()) // if something is trying to kill this job, the lock
        return; // will fail, in which case we'd better return..

    for (int i = 0; i < observers.size(); i++)
        observers[i]->eventNotification(event);
}
