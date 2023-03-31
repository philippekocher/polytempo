#pragma once
#include "JuceHeader.h"

#if defined(POLYTEMPO_NETWORK) || defined(POLYTEMPO_COMPOSER)
#define USING_SCORE
#endif

#ifdef USING_SCORE
#include "../Data/Polytempo_Score.h"
#endif

#include "Polytempo_ScoreScheduler.h"

class Polytempo_ScoreScheduler;

class Polytempo_ScoreSchedulerEngine : public Thread
{
public:
    Polytempo_ScoreSchedulerEngine() : Thread("Polytempo_ScoreScheduler_Thread"),
#ifdef USING_SCORE
                                       score(nullptr),
#endif
                                       scoreScheduler(nullptr), scoreTime(0),
                                       killed(false),
                                       shouldStop(true),
                                       pausing(false),
                                       shouldReturnToLastDownbeat(false),
                                       scoreTimeOffset(0)
    {
    }

    void run() override = 0;

    bool isRunning()
    {
        return !shouldStop;
    }

    void setScheduler(Polytempo_ScoreScheduler* theScheduler)
    {
        scoreScheduler = theScheduler;
    }

#ifdef USING_SCORE
    void setScore(Polytempo_Score* theScore)
    {
        score = theScore;
    }
#endif

    void stop(bool returnToDownbeat = false)
    {
        shouldStop = true;
        shouldReturnToLastDownbeat = returnToDownbeat;
    }

    void kill()
    {
        killed = true;
        stop();
    }

    void pause(bool shouldPause)
    {
        pausing = shouldPause;
    }

    bool isPausing()
    {
        return pausing;
    }

    virtual void setScoreTime(int time) = 0;

    int getScoreTime()
    {
        return int(scoreTime);
    }

    void setTempoFactor(float factor)
    {
        tempoFactor = factor;
    }

protected:
    int scoreTimeIncrement()
    {
        int increment = pausing ? 0 : (int32)(Time::getMillisecondCounter() - scoreTimeOffset);
        scoreTimeOffset = Time::getMillisecondCounter();
        return increment;
    }

#ifdef USING_SCORE
    Polytempo_Score* score;
#endif

    Polytempo_ScoreScheduler* scoreScheduler;
    double scoreTime; // the current time in the score
    double tempoFactor = 1;
    bool killed;
    bool shouldStop;
    bool pausing;
    bool shouldReturnToLastDownbeat;

private:
    uint32 scoreTimeOffset;
};

/*
   Both programmes need different schedulers.
*/

class Polytempo_ComposerEngine : public Polytempo_ScoreSchedulerEngine
{
public:
    void setScoreTime(int time) override;
    void run() override;
};

class Polytempo_NetworkEngine : public Polytempo_ScoreSchedulerEngine
{
public:
    Polytempo_NetworkEngine(): waitBeforeStart(0), lastDownbeat(0)
    {
    }

    void setScoreTime(int time) override;
    void run() override;

private:
    float waitBeforeStart;
    int lastDownbeat;
};
