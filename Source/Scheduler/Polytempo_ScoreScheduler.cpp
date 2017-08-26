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

#include "Polytempo_ScoreScheduler.h"
#include "Polytempo_EventScheduler.h"
#include "Polytempo_EventDispatcher.h"
#include "../Network/Polytempo_OSCSender.h"

Polytempo_ScoreScheduler::Polytempo_ScoreScheduler()
{}

Polytempo_ScoreScheduler::~Polytempo_ScoreScheduler()
{
    // allow the thread 2 seconds to stop cleanly - should be plenty of time.
    engine->stopThread(2000);
    
    engine = nullptr;

    clearSingletonInstance();
}

juce_ImplementSingleton (Polytempo_ScoreScheduler);

void Polytempo_ScoreScheduler::setEngine(Polytempo_SchedulerEngine* theEngine)
{
    engine = theEngine;
    engine->setScheduler(this);
}

bool Polytempo_ScoreScheduler::isRunning()
{
    return engine->isRunning() && !engine->isPausing();
}


// ----------------------------------------------------

void Polytempo_ScoreScheduler::eventNotification(Polytempo_Event *event)
{
    if(event == nullptr) return;
    if(event->getType() == eventType_Start)            start();
    else if(event->getType() == eventType_Stop)        stop();
    else if(event->getType() == eventType_Pause)       pause(event);
    else if(event->getType() == eventType_GotoMarker)  gotoMarker(event);
    else if(event->getType() == eventType_GotoLocator) gotoLocator(event);
    else if(event->getType() == eventType_TempoFactor) setTempoFactor(event);
}

// ----------------------------------------------------
#pragma mark -
#pragma mark transport

void Polytempo_ScoreScheduler::startStop()
{
    if(!engine->isRunning())        start();
    else if(engine->isPausing())    start();
    else                            stop();
}

void Polytempo_ScoreScheduler::start()
{
    DBG("start");
    if(!score)                    return;
    if(engine->isRunning() &&
       !engine->isPausing())      return;
    
    if(engine->isPausing()) engine->pause(false);
    else                    engine->startThread(10);  // 10 = highest priority
    
    // notify other components about this event
    // Polytempo_EventScheduler::getInstance()->scheduleEvent(Polytempo_Event::makeEvent(eventType_Start));
}

void Polytempo_ScoreScheduler::stop()
{
    DBG("stop");
    engine->stop();
    engine->pause(false);

    // notify other components that the scheduler has stopped
    // Polytempo_EventScheduler::getInstance()->scheduleEvent(Polytempo_Event::makeEvent(eventType_Stop));
}

void Polytempo_ScoreScheduler::pause(Polytempo_Event* event)
{
    DBG("pause");
    engine->pause(true);
  
    if(!event->getProperty("resume").isVoid())
        gotoLocator(Polytempo_Event::makeEvent(eventType_GotoLocator, event->getProperty("resume")));

    // notify other components about this event
    Polytempo_EventScheduler::getInstance()->scheduleEvent(event, true);
}

void Polytempo_ScoreScheduler::returnToLocator()
{
    if(!score)                     return;
    
    Polytempo_EventDispatcher::getInstance()->broadcastEvent(Polytempo_Event::makeEvent(eventType_GotoLocator, var(storedLocator)));
}

void Polytempo_ScoreScheduler::returnToBeginning()
{
    if(!score)                     return;
    
    Polytempo_Event *first_Event = score->getFirstEvent();

    if(first_Event) storedLocator = first_Event->getTime();
    else            storedLocator = 0;
    
    Polytempo_EventDispatcher::getInstance()->broadcastEvent(Polytempo_Event::makeEvent(eventType_GotoLocator, var(storedLocator)));
}

void Polytempo_ScoreScheduler::skipToEvent(Polytempo_EventType type, bool backwards)
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

bool Polytempo_ScoreScheduler::gotoMarker(Polytempo_Event *event, bool storeLocator)
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

void Polytempo_ScoreScheduler::gotoLocator(Polytempo_Event *event)
{
    if(!score) return;
    if(engine->isRunning() && !engine->isPausing())  stop();
    
    //DBG("goto locator "<<event->getProperty("value").toString());
    float locator = event->getProperty("value");
    engine->setLocator(locator);
    
    // update locator in all components
    Polytempo_Event *schedulerTick = Polytempo_Event::makeEvent(eventType_Tick);
    schedulerTick->setTime(locator);
    Polytempo_EventScheduler::getInstance()->scheduleEvent(schedulerTick);
}

void Polytempo_ScoreScheduler::storeLocator(float loc)
{
    storedLocator = loc;
}

void Polytempo_ScoreScheduler::setTempoFactor(Polytempo_Event *event)
{
    engine->setTempoFactor(event->getProperty("value"));
    
    // notify other components about this event
    Polytempo_EventScheduler::getInstance()->scheduleEvent(event);
}

void Polytempo_ScoreScheduler::executeInit()
{
    OwnedArray < Polytempo_Event >& initEvents = score->getInitEvents();
    for(int i=0;i<initEvents.size();i++)
    {
        Polytempo_EventScheduler::getInstance()->scheduleEvent(initEvents.getUnchecked(i), true);
    }
}

void Polytempo_ScoreScheduler::setScore(Polytempo_Score* theScore)
{
    // reset / delete everything that belongs to an old score
    engine->kill();
    Polytempo_EventScheduler::getInstance()->scheduleEvent(Polytempo_Event::makeEvent(eventType_DeleteAll));
    
    // set new score
    score = theScore;
    engine->setScore(theScore);
    
    executeInit();
    
    // set default section
    score->setSection();
    Polytempo_EventScheduler::getInstance()->scheduleEvent(Polytempo_Event::makeEvent(eventType_Ready));

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
