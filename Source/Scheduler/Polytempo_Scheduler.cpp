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

#include "Polytempo_Scheduler.h"
#include "Polytempo_EventDispatcher.h"
#include "../Network/Polytempo_OSCSender.h"

Polytempo_Scheduler::Polytempo_Scheduler()
{}

Polytempo_Scheduler::~Polytempo_Scheduler()
{
    // allow the thread 2 seconds to stop cleanly - should be plenty of time.
    engine->stopThread(2000);
    
    engine = nullptr;

    clearSingletonInstance();
}

juce_ImplementSingleton (Polytempo_Scheduler);

void Polytempo_Scheduler::setEngine(Polytempo_SchedulerEngine* theEngine)
{
    engine = theEngine;
    engine->setScheduler(this);
}

bool Polytempo_Scheduler::isRunning()
{
    return engine->isRunning() && !engine->isPausing();
}


// ----------------------------------------------------

void Polytempo_Scheduler::handleEvent(Polytempo_Event *event, int deferTime)
{
    if(event == nullptr) return;
    
    if(deferTime > 0) DBG("schedule event...");
        
    // events handled by the scheduler

    if(event->getType() == eventType_Start)            start();
    else if(event->getType() == eventType_Stop)        stop();
    else if(event->getType() == eventType_Pause)       pause(event);
    else if(event->getType() == eventType_GotoMarker)  gotoMarker(event);
    else if(event->getType() == eventType_GotoLocator) gotoLocator(event);
    else if(event->getType() == eventType_TempoFactor) setTempoFactor(event);
    
    
    // events handled by other objects
    
    else notify(event);
}


// ----------------------------------------------------
#pragma mark -
#pragma mark notify event observers

void Polytempo_Scheduler::registerObserver(Polytempo_EventObserver *obs)
{
    observers.add(obs);
}

void Polytempo_Scheduler::removeObserver(Polytempo_EventObserver *obs)
{
    observers.removeAllInstancesOf(obs);
}

void Polytempo_Scheduler::notify(Polytempo_Event* event)
{
    // because this is a background thread, we mustn't do any UI work without
    // first grabbing a MessageManagerLock..
    const MessageManagerLock mml (Thread::getCurrentThread());
    
    if (! mml.lockWasGained())  // if something is trying to kill this job, the lock
        return;                 // will fail, in which case we'd better return..

    for (int i = 0; i < observers.size(); i++)
        observers[i]->eventNotification(event);
}

// ----------------------------------------------------
#pragma mark -
#pragma mark transport

void Polytempo_Scheduler::startStop()
{
    if(!engine->isRunning())        start();
    else if(engine->isPausing())    start();
    else                            stop();
}

void Polytempo_Scheduler::start()
{
    DBG("start");
    if(!score)                    return;
    if(engine->isRunning() &&
       !engine->isPausing())      return;
    
    if(engine->isPausing()) engine->pause(false);
    else                    engine->startThread(10);  // 10 = highest priority
    
    // notify other components about this event
    notify(Polytempo_Event::makeEvent(eventType_Start));
}

void Polytempo_Scheduler::stop()
{
    DBG("stop");
    engine->stop();
    engine->pause(false);

    // notify other components that the scheduler has stopped
    notify(Polytempo_Event::makeEvent(eventType_Stop));
}

void Polytempo_Scheduler::pause(Polytempo_Event* event)
{
    DBG("pause");
    engine->pause(true);
  
    if(!event->getProperty("resume").isVoid())
        gotoLocator(Polytempo_Event::makeEvent(eventType_GotoLocator, event->getProperty("resume")));

    // notify other components about this event
    notify(event);
}

void Polytempo_Scheduler::returnToLocator()
{
    if(!score)                     return;
    
    Polytempo_EventDispatcher::getInstance()->broadcastEvent(Polytempo_Event::makeEvent(eventType_GotoLocator, var(storedLocator)));
}

void Polytempo_Scheduler::returnToBeginning()
{
    if(!score)                     return;
    
    Polytempo_Event *first_Event = score->getFirstEvent();

    if(first_Event) storedLocator = first_Event->getTime();
    else            storedLocator = 0;
    
    Polytempo_EventDispatcher::getInstance()->broadcastEvent(Polytempo_Event::makeEvent(eventType_GotoLocator, var(storedLocator)));
}

void Polytempo_Scheduler::skipToEvent(Polytempo_EventType type, bool backwards)
{
    if(!score) return;

    Polytempo_Event *event = score->searchEvent(engine->getLocator(),
                                                type, backwards);

    if(event != nullptr)
    {
        storedLocator = event->getTime();
        Polytempo_EventDispatcher::getInstance()->broadcastEvent(Polytempo_Event::makeEvent(eventType_GotoLocator, storedLocator));
    }
}

bool Polytempo_Scheduler::gotoMarker(Polytempo_Event *event, bool storeLocator)
{
    if(!score) return false;
    
    //DBG("goto marker "<<event->getProperty("value").toString());
    float locator;
    
    if(score->getLocatorForMarker(event->getProperty("value"), &locator))
    {
        if(storeLocator)
        {
            storedLocator = locator;
        }
        
        Polytempo_EventDispatcher::getInstance()->broadcastEvent(Polytempo_Event::makeEvent(eventType_GotoLocator, var(locator)));

        return true;
    }

    return false;
}

void Polytempo_Scheduler::gotoLocator(Polytempo_Event *event)
{
    if(!score) return;
    if(engine->isRunning() && !engine->isPausing())  stop();
    
    //DBG("goto locator "<<event->getProperty("value").toString());
    float locator = event->getProperty("value");
    engine->setLocator(locator);
    
    // update locator in all components
    Polytempo_Event *schedulerTick = Polytempo_Event::makeEvent(eventType_Tick);
    schedulerTick->setTime(locator);
    notify(schedulerTick);
}

void Polytempo_Scheduler::storeLocator(float loc)
{
    storedLocator = loc;
}

void Polytempo_Scheduler::setTempoFactor(Polytempo_Event *event)
{
    engine->setTempoFactor(event->getProperty("value"));
    
    // notify other components about this event
    notify(event);
}

void Polytempo_Scheduler::setScore(Polytempo_Score* theScore)
{
    // reset / delete everything that belongs to an old score
    engine->kill();
    notify(Polytempo_Event::makeEvent(eventType_DeleteAll));
    
    // set new score
    score = theScore;
    engine->setScore(theScore);
    
    // execute init section
    OwnedArray < Polytempo_Event >& initEvents = score->getInitEvents();
    for(int i=0;i<initEvents.size();i++)
    {
        notify(initEvents.getUnchecked(i));
    }
    
    // set default section
    score->setSection();
    notify(Polytempo_Event::makeEvent(eventType_Ready));

    // goto beginning of score (only local);
    if(score->getFirstEvent())
    {
        storeLocator(score->getFirstEvent()->getTime());
        gotoLocator(Polytempo_Event::makeEvent(eventType_GotoLocator, score->getFirstEvent()->getTime()));
    }
    else
    {
        storeLocator(0);
        gotoLocator(Polytempo_Event::makeEvent(eventType_GotoLocator, 0));
    }
}