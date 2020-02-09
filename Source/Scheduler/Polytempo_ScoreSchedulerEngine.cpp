#include "Polytempo_ScoreSchedulerEngine.h"
#include "Polytempo_Event.h"
#include "../Data/Polytempo_Composition.h"
#include "Polytempo_EventDispatcher.h"
#include "Polytempo_EventScheduler.h"
#include "../Network/Polytempo_TimeProvider.h"

#ifdef POLYTEMPO_COMPOSER
void Polytempo_ComposerEngine::setScoreTime(int time)
{
    scoreTime = time;
    score->setTime(time);
}

void Polytempo_ComposerEngine::run()
{
    //DBG("run");
    int interval = 100;
    int lookAhead = 2000;
    int syncTime;
    
    Polytempo_Event *nextScoreEvent = score->getNextEvent();
    Polytempo_Event *nextOscEvent;
    Polytempo_Event *schedulerTick  = Polytempo_Event::makeEvent(eventType_Tick);
    
    schedulerTick->setOwned(true);
    
    scoreTimeIncrement(); // reset timer
    killed = false;
    shouldStop = false;
    
    while(!threadShouldExit() && !shouldStop)
    {
        scoreTime += int(scoreTimeIncrement() * tempoFactor);
        
        while(nextScoreEvent &&
              nextScoreEvent->getTime() <= scoreTime + lookAhead)
        {
            if(nextScoreEvent->hasDefinedTime())
            {
                // calculate syncTime
            
                syncTime = Time::getMillisecondCounter();
                syncTime += int((nextScoreEvent->getTime() - scoreTime) / tempoFactor);
                
                if(nextScoreEvent->hasProperty(eventPropertyString_Defer))
                    syncTime += int(float(nextScoreEvent->getProperty(eventPropertyString_Defer)) * 1000.0f);
                
                nextScoreEvent->setSyncTime(syncTime);
                
                if(nextScoreEvent->getType() == eventType_Beat)
                {
                    // add playback properties to next event
                    int sequenceIndex = nextScoreEvent->getProperty("~sequence");
                    Polytempo_Sequence* sequence = Polytempo_Composition::getInstance()->getSequence(sequenceIndex);
     
                    nextScoreEvent = new Polytempo_Event(*nextScoreEvent); // we need a copy to add the playback properties
                    nextScoreEvent->setOwned(false); // this event can be deleted by the scheduler after its use
                    sequence->addPlaybackPropertiesToEvent(nextScoreEvent);

                    // next osc event
                    nextOscEvent = sequence->getOscEvent(nextScoreEvent);
                    Polytempo_EventScheduler::getInstance()->scheduleScoreEvent(nextOscEvent);
                }
            
                Polytempo_EventScheduler::getInstance()->scheduleScoreEvent(nextScoreEvent);
            }
            
            // get next event
            nextScoreEvent = score->getNextEvent();
        }
        
        schedulerTick->setValue(scoreTime * 0.001f);
        Polytempo_EventScheduler::getInstance()->scheduleScoreEvent(schedulerTick);
        
        wait(interval);
    }
    
    Polytempo_Composition::getInstance()->updateScore();
    
    /*
         set time in order to set the score's pointer to the next event
         (which is already ahead by the amount of lookahead).
	*/
    if(!killed) scoreScheduler->gotoTime(scoreTime);
}
#endif

//-------------------------------------------------------------------------------------
#ifdef POLYTEMPO_NETWORK
void Polytempo_NetworkEngine::setScoreTime(int time)
{
    scoreTime = time;

    Array<Polytempo_Event*> events; // the events to execute immediately
    score->setTime(time, &events, &waitBeforeStart);

    Polytempo_EventScheduler* scheduler = Polytempo_EventScheduler::getInstance();
    std::unique_ptr<Polytempo_Event> clearAllEvent = std::unique_ptr<Polytempo_Event>(Polytempo_Event::makeEvent(eventType_ClearAll));
    scheduler->executeEvent(clearAllEvent.get());

    for (int i = 0; i < events.size(); i++)
    {
        scheduler->executeEvent(events[i]);
    }

    lastDownbeat = time;
}

void Polytempo_NetworkEngine::run()
{
    //DBG("run");
    int interval = 200;
    int lookAhead = 200;

    Polytempo_Event* nextScoreEvent = score->getNextEvent();
    Polytempo_Event* schedulerTick = Polytempo_Event::makeEvent(eventType_Tick);

    schedulerTick->setOwned(true);

    scoreTimeIncrement(); // reset timer
    killed = false;
    shouldStop = false;
    pausing = false;

    while (!threadShouldExit() && !shouldStop)
    {
        scoreTime += int(double(scoreTimeIncrement()) * tempoFactor);

        while (!shouldStop && !pausing && nextScoreEvent && nextScoreEvent->getTime() <= scoreTime + lookAhead)
        {
            // calculate syncTime

            uint32 syncTime;
            Polytempo_TimeProvider::getInstance()->getSyncTime(&syncTime);
            syncTime += int(double(nextScoreEvent->getTime() - scoreTime) / tempoFactor);

            if (nextScoreEvent->hasProperty(eventPropertyString_Defer))
                syncTime += int(float(nextScoreEvent->getProperty(eventPropertyString_Defer)) * 1000.0f);

            nextScoreEvent->setSyncTime(syncTime);

            Polytempo_EventScheduler::getInstance()->scheduleScoreEvent(nextScoreEvent);

            if (nextScoreEvent->getType() == eventType_Beat && int(nextScoreEvent->getProperty(eventPropertyString_Pattern)) < 20)
                lastDownbeat = nextScoreEvent->getTime();

            // get next event
            nextScoreEvent = score->getNextEvent();
        }

        schedulerTick->setValue(scoreTime * 0.001f);
        Polytempo_EventScheduler::getInstance()->scheduleScoreEvent(schedulerTick);

        wait(interval);
    }

    delete schedulerTick;

    /*
       return to beginning of the current bar (last downbeat)
       this must be called here to make sure that the engine thread really
       has finished.
    */
    if (!killed) scoreScheduler->gotoTime(lastDownbeat);
}
#endif
