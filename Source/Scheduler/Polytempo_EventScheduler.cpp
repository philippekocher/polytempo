/* ==============================================================================
 
 This file is part of the POLYTEMPO software package.
 Copyright (c) 2016 - Zurich University of the Arts,
 ICST Institute for Computer Music and Sound Technology
 http://www.icst.net
 
 Author: Philippe Kocher
 
 POLYTEMPO is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 POLYTEMPO is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this software. If not, see <http://www.gnu.org/licenses/>.
 
 ============================================================================== */

#include "Polytempo_EventScheduler.h"
#include "../Network/Polytempo_TimeProvider.h"


Polytempo_EventScheduler::Polytempo_EventScheduler() : Thread("Polytempo_EventScheduler_Thread")
{}

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

void Polytempo_EventScheduler::scheduleScoreEvent(Polytempo_Event *event)
{
    if(event == nullptr) return;
    if(event->getType() == eventType_None) return;
    
	uint32 currentSyncTime;
	Polytempo_TimeProvider::getInstance()->getSyncTime(&currentSyncTime);
	if(event->getSyncTime() <= currentSyncTime)
    {
        notify(event);
        if(!event->isOwned()) delete event;
    }
    else
    {
        scheduledScoreEvents.add(event);
    }
    // DBG("scheduled score events: "<<scheduledScoreEvents.size());
}

void Polytempo_EventScheduler::deletePendingEvents()
{
    deleteEvents = true;
}

void Polytempo_EventScheduler::scheduleEvent(Polytempo_Event *event)
{
    if(event == nullptr) return;
    if(event->getType() == eventType_None) return;
    
	uint32 currentSyncTime;
	Polytempo_TimeProvider::getInstance()->getSyncTime(&currentSyncTime);
	if(event->getSyncTime() <= currentSyncTime)
    {
        notify(event);
        if(!event->isOwned()) delete event;
    }
    else
    {
        scheduledEvents.add(event);
    }
   // DBG("scheduled events: "<<scheduledEvents.size());
}

// ----------------------------------------------------
#pragma mark -
#pragma mark thread

void Polytempo_EventScheduler::run()
{
    int interval = 5;
    uint32 currentSyncTime;
	
    while(!threadShouldExit())
    {
        // get current sync time
		Polytempo_TimeProvider::getInstance()->getSyncTime(&currentSyncTime);
		if(deleteEvents)
        {
            scheduledScoreEvents.clear();
            scheduledEvents.clear();
            deleteEvents = false;
        }
        
        for(int i=0;i<scheduledScoreEvents.size();)
        {
            Polytempo_Event *event = scheduledScoreEvents[i];
            
            if(event->getSyncTime() <= currentSyncTime)
            {
                notify(event);
                if(!event->isOwned()) delete event;
                scheduledScoreEvents.remove(i);
            }
            else ++i;
        }
        for(int j=0;j<scheduledEvents.size();)
        {
            Polytempo_Event *event = scheduledEvents[j];
            
            if(event->getSyncTime() <= currentSyncTime)
            {
                notify(event);
                if(!event->isOwned()) delete event;
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

void Polytempo_EventScheduler::registerObserver(Polytempo_EventObserver *obs)
{
    observers.add(obs);
}

void Polytempo_EventScheduler::removeObserver(Polytempo_EventObserver *obs)
{
    observers.removeAllInstancesOf(obs);
}

void Polytempo_EventScheduler::notify(Polytempo_Event* event)
{
    // because this is a background thread, we mustn't do any UI work without
    // first grabbing a MessageManagerLock..
    const MessageManagerLock mml (Thread::getCurrentThread());
    
    if (! mml.lockWasGained())  // if something is trying to kill this job, the lock
        return;                 // will fail, in which case we'd better return..
    
    for (int i = 0; i < observers.size(); i++)
        observers[i]->eventNotification(event);
}
