#include "Polytempo_ScoreScheduler.h"
#include "Polytempo_EventScheduler.h"
#include "Polytempo_EventDispatcher.h"
#include "../Network/Polytempo_InterprocessCommunication.h"

Polytempo_ScoreScheduler::Polytempo_ScoreScheduler()
{
}

Polytempo_ScoreScheduler::~Polytempo_ScoreScheduler()
{
    // allow the thread 2 seconds to stop cleanly - should be plenty of time.
    engine->stopThread(2000);

    engine = nullptr;

    clearSingletonInstance();
}

juce_ImplementSingleton (Polytempo_ScoreScheduler)

void Polytempo_ScoreScheduler::setEngine(Polytempo_ScoreSchedulerEngine* theEngine)
{
    engine.reset(theEngine);
    engine->setScheduler(this);
}

bool Polytempo_ScoreScheduler::isRunning()
{
    return engine->isRunning();
}

// ----------------------------------------------------

void Polytempo_ScoreScheduler::eventNotification(Polytempo_Event* event)
{
    if (event == nullptr) return;
    if (event->getType() == eventType_Start) start();
    else if (event->getType() == eventType_Stop) stop(true);
    else if (event->getType() == eventType_Pause) pause(event);
    else if (event->getType() == eventType_GotoMarker) gotoMarker(event);
    else if (event->getType() == eventType_GotoTime) gotoTime(event);
    else if (event->getType() == eventType_TempoFactor) setTempoFactor(event);
    else if(event->getType() == eventType_Ready)
    {
#if defined (USING_SCORE) && defined (POLYTEMPO_NETWORK)
        MessageManager::callAsync([this]() {
            score->setReady();
            // goto beginning of score (only local);
            int time = score->getFirstEvent() != nullptr ? score->getFirstEvent()->getTime() : 0;
            storeLocator(time);
            gotoTime(time);
        });
#else
        MessageManager::callAsync([this]() { gotoTime((0)); });
#endif
    }
}

// ----------------------------------------------------
#pragma mark -
#pragma mark playback

void Polytempo_ScoreScheduler::startStop()
{
    if (isRunning() && !engine->isPausing())
        Polytempo_EventDispatcher::getInstance()->broadcastEvent(Polytempo_Event::makeEvent(eventType_Stop));
    else
        Polytempo_EventDispatcher::getInstance()->broadcastEvent(Polytempo_Event::makeEvent(eventType_Start));
}

void Polytempo_ScoreScheduler::broadcastStop()
{
    Polytempo_EventDispatcher::getInstance()->broadcastEvent(Polytempo_Event::makeEvent(eventType_Stop));
}

void Polytempo_ScoreScheduler::start()
{
#ifdef USING_SCORE
    if (!score) return;
#endif
    if (engine->isRunning() &&
        !engine->isPausing())
        return;

    if (engine->isPausing()) engine->pause(false);
    else engine->startThread(Thread::Priority::normal);
}

void Polytempo_ScoreScheduler::stop(bool returnToDownbeat)
{
    engine->stop(returnToDownbeat);
    engine->pause(false);
    Polytempo_EventScheduler::getInstance()->deletePendingEvents();
}

void Polytempo_ScoreScheduler::pause(Polytempo_Event* event)
{
    engine->pause(true);

    if (!event->getProperty("resume").isVoid())
        gotoTime(Polytempo_Event::makeEvent(eventType_GotoTime, event->getProperty("resume")));
}

void Polytempo_ScoreScheduler::kill()
{
    engine->kill();
}

void Polytempo_ScoreScheduler::returnToLocator()
{
#ifdef USING_SCORE
    if (!score) return;
#endif

    Polytempo_EventDispatcher::getInstance()->broadcastEvent(Polytempo_Event::makeEvent(eventType_GotoTime, var(storedLocator * 0.001f)));
}

void Polytempo_ScoreScheduler::returnToBeginning()
{
#ifdef USING_SCORE
    if (!score) return;

    Polytempo_Event* first_Event = score->getFirstEvent();

    if (first_Event) storedLocator = first_Event->getTime();
    else storedLocator = 0;
#else
    storedLocator = 0;
#endif
    Polytempo_EventDispatcher::getInstance()->broadcastEvent(Polytempo_Event::makeEvent(eventType_GotoTime, var(storedLocator * 0.001f)));
}

#ifdef USING_SCORE
void Polytempo_ScoreScheduler::skipToEvent(Polytempo_EventType type, bool backwards)
{
    if (!score) return;

    Polytempo_Event* event = score->searchEvent(engine->getScoreTime(),
                                                type, backwards);

    if (event != nullptr)
    {
        storedLocator = event->getTime();
        Polytempo_EventDispatcher::getInstance()->broadcastEvent(Polytempo_Event::makeEvent(eventType_GotoTime, storedLocator * 0.001f));
    }
}
#else
void Polytempo_ScoreScheduler::skipToEvent(Polytempo_EventType, bool) {}
#endif

bool Polytempo_ScoreScheduler::gotoMarker(Polytempo_Event* event, bool storeLocator)
{
    return gotoMarker(event->getProperty(eventPropertyString_Value), storeLocator);
}

#ifdef USING_SCORE
bool Polytempo_ScoreScheduler::gotoMarker(String marker, bool storeLocator)
{
    if (!score) return false;

    int time;

    if (score->getTimeForMarker(marker, &time))
    {
        if (storeLocator)
        {
            storedLocator = time;
        }

        Polytempo_EventDispatcher::getInstance()->broadcastEvent(Polytempo_Event::makeEvent(eventType_GotoTime, var(time * 0.001f)));

        return true;
    }

    return false;
}
#else
bool Polytempo_ScoreScheduler::gotoMarker(String, bool) { return false; }
#endif

void Polytempo_ScoreScheduler::gotoTime(Polytempo_Event* event)
{
    gotoTime(int((float)event->getProperty(eventPropertyString_Value) * 1000.0f));
}

void Polytempo_ScoreScheduler::gotoTime(int time)
{
#ifdef USING_SCORE
    if (!score) return;
#endif
    if (engine->isRunning() && !engine->isPausing()) stop();

    engine->setScoreTime(time);

    // update locator in all components
    std::unique_ptr<Polytempo_Event> schedulerTick = std::unique_ptr<Polytempo_Event>(Polytempo_Event::makeEvent(eventType_Tick));
    schedulerTick->setValue(time * 0.001f);
    Polytempo_EventScheduler::getInstance()->executeEvent(schedulerTick.get());
}

void Polytempo_ScoreScheduler::storeLocator(int loc)
{
    storedLocator = loc;
}

void Polytempo_ScoreScheduler::setTempoFactor(Polytempo_Event* event)
{
    engine->setTempoFactor(event->getProperty(eventPropertyString_Value));
}

void Polytempo_ScoreScheduler::executeInit()
{
#ifdef USING_SCORE
    if (!score) return;

    OwnedArray<Polytempo_Event>* initEvents = score->getInitEvents();
    if (initEvents != nullptr && !initEvents->isEmpty())
    {
        score->setReady(false);

        int size = initEvents->size();
        for (int i = 0; i < size; i++)
        {
            Polytempo_EventScheduler::getInstance()->scheduleInitEvent(initEvents->getUnchecked(i));
        }
    }
#endif
    Polytempo_EventScheduler::getInstance()->scheduleInitEvent(Polytempo_Event::makeEvent(eventType_Ready));
}

#ifdef USING_SCORE
void Polytempo_ScoreScheduler::setScore(Polytempo_Score* theScore)
{
    // set new score
    newScore = theScore;

    // reset / delete everything that belongs to an old score
    if(engine->isRunning()) {
        Polytempo_EventScheduler::getInstance()->deletePendingEvents();
        engine->kill();
    }
    else swapScores();
}

void Polytempo_ScoreScheduler::swapScores()
{
    if (score == newScore) return;
    
    Polytempo_EventScheduler::getInstance()->scheduleEvent(Polytempo_Event::makeEvent(eventType_DeleteAll));
    Polytempo_EventScheduler::getInstance()->scheduleEvent(Polytempo_Event::makeEvent(eventType_TempoFactor, 1.0));

    // set new score
    score = newScore;
    engine->setScore(score);

    executeInit();

    // set default section
    score->selectSection();
}
#endif

int Polytempo_ScoreScheduler::getScoreTime()
{
    return engine->getScoreTime();
}
