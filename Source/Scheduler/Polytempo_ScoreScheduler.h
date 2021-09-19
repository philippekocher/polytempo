#pragma once

#include "Polytempo_ScoreSchedulerEngine.h"
#ifdef USING_SCORE
#include "../Data/Polytempo_Score.h"
#endif
#include "../Scheduler/Polytempo_EventObserver.h"

class Polytempo_ScoreSchedulerEngine;

class Polytempo_ScoreScheduler : public Polytempo_EventObserver
{
public:
    Polytempo_ScoreScheduler();
    ~Polytempo_ScoreScheduler() override;

    juce_DeclareSingleton(Polytempo_ScoreScheduler, false)

    void setEngine(Polytempo_ScoreSchedulerEngine*);
    bool isRunning();

    void eventNotification(Polytempo_Event* event) override;

    /* playback
     --------------------------------------- */
    void startStop();
    void broadcastStop();
    void start();
    void stop();
    void pause(Polytempo_Event*);
    void kill();
    void returnToLocator();
    void returnToBeginning();
    bool gotoMarker(Polytempo_Event* event, bool storeLocator = true);
    bool gotoMarker(String marker, bool storeLocator = true);
    void skipToEvent(Polytempo_EventType type, bool backwards = false);
    void gotoTime(Polytempo_Event* event);
    void gotoTime(int time);
    void storeLocator(int loc);
    void setTempoFactor(Polytempo_Event* event);
    void executeInit();

    /* accessors
     --------------------------------------- */
#ifdef USING_SCORE
    void setScore(Polytempo_Score* score_);
#endif
    int getScoreTime();

private:
    std::unique_ptr<Polytempo_ScoreSchedulerEngine> engine;
    Array<class Polytempo_EventObserver *> observers;

    int storedLocator = 0; // milliseconds

#ifdef USING_SCORE
    Polytempo_Score* score = nullptr;
    AlertWindow* loadStatusWindow = nullptr;
#endif
};
