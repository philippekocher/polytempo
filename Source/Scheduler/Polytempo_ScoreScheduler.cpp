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
    else if (event->getType() == eventType_Stop) stop();
    else if (event->getType() == eventType_Pause) pause(event);
    else if (event->getType() == eventType_GotoMarker) gotoMarker(event);
    else if (event->getType() == eventType_GotoTime) gotoTime(event);
    else if (event->getType() == eventType_TempoFactor) setTempoFactor(event);
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
    if (!score) return;
    if (engine->isRunning() &&
        !engine->isPausing())
        return;

    if (engine->isPausing()) engine->pause(false);
    else engine->startThread(5); // priority between 0 and 10
}

void Polytempo_ScoreScheduler::stop()
{
    engine->stop();
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
    if (!score) return;

    Polytempo_EventDispatcher::getInstance()->broadcastEvent(Polytempo_Event::makeEvent(eventType_GotoTime, var(storedLocator * 0.001f)));
}

void Polytempo_ScoreScheduler::returnToBeginning()
{
    if (!score) return;

    Polytempo_Event* first_Event = score->getFirstEvent();

    if (first_Event) storedLocator = first_Event->getTime();
    else storedLocator = 0;

    Polytempo_EventDispatcher::getInstance()->broadcastEvent(Polytempo_Event::makeEvent(eventType_GotoTime, var(storedLocator * 0.001f)));
}

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

bool Polytempo_ScoreScheduler::gotoMarker(Polytempo_Event* event, bool storeLocator)
{
    return gotoMarker(event->getProperty(eventPropertyString_Value), storeLocator);
}

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

void Polytempo_ScoreScheduler::gotoTime(Polytempo_Event* event)
{
    gotoTime(int((float)event->getProperty(eventPropertyString_Value) * 1000.0f));
}

void Polytempo_ScoreScheduler::gotoTime(int time)
{
    if (!score) return;
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
    if (!score) return;

    OwnedArray<Polytempo_Event>* initEvents = score->getInitEvents();
    if (initEvents != nullptr)
    {
        for (int i = 0; i < initEvents->size(); i++)
        {
            Polytempo_EventScheduler::getInstance()->scheduleScoreEvent(initEvents->getUnchecked(i));
        }
    }
}

void Polytempo_ScoreScheduler::setScore(Polytempo_Score* theScore)
{
    // reset / delete everything that belongs to an old score
    engine->kill();
    Polytempo_EventScheduler::getInstance()->scheduleEvent(Polytempo_Event::makeEvent(eventType_DeleteAll));
    Polytempo_EventScheduler::getInstance()->scheduleEvent(Polytempo_Event::makeEvent(eventType_TempoFactor, 1.0));

    // set new score
    score = theScore;
    engine->setScore(theScore);

    executeInit();

    // set default section
    score->selectSection();
    Polytempo_EventScheduler::getInstance()->scheduleEvent(Polytempo_Event::makeEvent(eventType_Ready));

    // goto beginning of score (only local);
    if (score->getFirstEvent())
    {
        storeLocator(score->getFirstEvent()->getTime());
        gotoTime(score->getFirstEvent()->getTime());
    }
    else
    {
        storeLocator(0);
        gotoTime(0);
    }
}

int Polytempo_ScoreScheduler::getScoreTime()
{
    return engine->getScoreTime();
}
