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

#include "Polytempo_SchedulerEngine.h"
#include "Polytempo_Event.h"
#include "../Data/Polytempo_Composition.h"
#include "Polytempo_EventDispatcher.h"


#ifdef POLYTEMPO_COMPOSER
void Polytempo_ComposerEngine::setLocator(float locator)
{
    milisecondLocator = locator * 1000 + 0.5;
    score->setLocator(locator);
}

void Polytempo_ComposerEngine::run()
{
    //DBG("run");
    int x=0;
    int interval = 5;
    
    Polytempo_Event *nextScoreEvent = score->getNextEvent();
    Polytempo_Event *nextOscEvent;
    Polytempo_Event *schedulerTick  = Polytempo_Event::makeEvent(eventType_Tick);
    
    timerIncrement(); // reset timer
    shouldStop = false;
    
    while(!threadShouldExit() && !shouldStop && nextScoreEvent)
    {
        milisecondLocator += timerIncrement() * tempoFactor;
        while(nextScoreEvent && nextScoreEvent->getMilisecondTime() <= milisecondLocator)
        {
            if(nextScoreEvent != nullptr && nextScoreEvent->getType() == eventType_Beat)
            {
                // add playback properties to next event
                int sequenceIndex = nextScoreEvent->getProperty("~sequence");
                nextScoreEvent = new Polytempo_Event(*nextScoreEvent); // get a copy
                Polytempo_Sequence* sequence = Polytempo_Composition::getInstance()->getSequence(sequenceIndex);
                
                sequence->addPlaybackPropertiesToEvent(nextScoreEvent);
                
                // next osc event
                nextOscEvent = sequence->getOscEvent(nextScoreEvent);
                scheduler->handleEvent(nextOscEvent, 0);
            }
 
            scheduler->handleEvent(nextScoreEvent, 0);
            
            // get next event
            nextScoreEvent = score->getNextEvent();
        }
        
        if(++x > 20 && !threadShouldExit())
        {
            schedulerTick->setTime(milisecondLocator * 0.001);
            scheduler->notify(schedulerTick);
            x=0;
        }
        
        wait(interval);
    }
    
    scheduler->handleEvent(Polytempo_Event::makeEvent(eventType_Stop));
    scheduler->gotoLocator(Polytempo_Event::makeEvent(eventType_GotoLocator, var(milisecondLocator * 0.001)));
}
#endif

//-------------------------------------------------------------------------------------
#ifdef POLYTEMPO_NETWORK
void Polytempo_NetworkEngine::setLocator(float locator)
{
    //DBG("set "<<locator);
    milisecondLocator = locator * 1000;

    Array <class Polytempo_Event*> events; // the events to execute immediately
    score->setLocator(locator, &events, &waitBeforeStart);
    scoreScheduler->notify(Polytempo_Event::makeEvent(eventType_ClearAll));
    for(int i=0;i<events.size();i++)
    {
        scoreScheduler->notify(events[i]);
    }
    
    lastDownbeat = locator;
}

void Polytempo_NetworkEngine::run()
{
    //DBG("run");
    int x=0;
    int interval = 5;
    
    Polytempo_Event *nextScoreEvent = score->getNextEvent();
    Polytempo_Event *schedulerTick  = Polytempo_Event::makeEvent(eventType_Tick);
    
    timerIncrement(); // reset timer
    killed = false;
    shouldStop = false;
    pausing = false;
    
    while(!threadShouldExit() && !shouldStop && nextScoreEvent)
    {
        milisecondLocator += timerIncrement() * tempoFactor;
        
        while(!shouldStop && !pausing && nextScoreEvent && nextScoreEvent->getMilisecondTime() <= milisecondLocator)
        {
            scoreScheduler->handleEvent(nextScoreEvent, 0);
            if(nextScoreEvent->getType() == eventType_Beat && int(nextScoreEvent->getProperty(eventPropertyString_Pattern)) < 20)
                lastDownbeat = nextScoreEvent->getTime();
            
            // get next event
            nextScoreEvent = score->getNextEvent();
            
        }
        
        if(++x > 20 && !shouldStop)
        {
            schedulerTick->setTime(milisecondLocator * 0.001f);
            scoreScheduler->notify(schedulerTick);
            x=0;
        }
        
        wait(interval);
    }

    /*
       return to beginning of the current bar (last downbeat)
       this must be called here to make sure that the engine thread really
       has finished.
    */
    if(!killed) scoreScheduler->gotoLocator(Polytempo_Event::makeEvent(eventType_GotoLocator, lastDownbeat));
}
#endif

