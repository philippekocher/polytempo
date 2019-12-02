#pragma once

#include "../Data/Polytempo_Score.h"
#include "Polytempo_ScoreScheduler.h"

class Polytempo_ScoreScheduler;

class Polytempo_ScoreSchedulerEngine : public Thread
{
public:
    Polytempo_ScoreSchedulerEngine() : Thread("Polytempo_ScoreScheduler_Thread") {};
    
    void run() = 0;
    bool isRunning()
    {
        return !shouldStop;
    }
    void setScheduler(Polytempo_ScoreScheduler* theScheduler)
    {
        scoreScheduler = theScheduler;
    }
    void setScore(Polytempo_Score* theScore)
    {
        score = theScore;
    }
    void stop()
    {
        shouldStop = true;
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
        return scoreTime;
    }
    void setTempoFactor(float factor)
    {
        tempoFactor = factor;
    }

protected:
    int scoreTimeIncrement()
    {
        int increment = pausing ? 0 : Time::getMillisecondCounter() - scoreTimeOffset;
        scoreTimeOffset = Time::getMillisecondCounter();
        return increment;
    }

    Polytempo_Score *score;
    Polytempo_ScoreScheduler* scoreScheduler;
    int scoreTime; // the current time in the score
    double tempoFactor = 1;
    bool killed;
    bool shouldStop;
    bool pausing;
    
private:
    int scoreTimeOffset;
};

/*
   Both programmes need different schedulers.
*/

class Polytempo_ComposerEngine : public Polytempo_ScoreSchedulerEngine
{
public:
    void stop();
    void setScoreTime(int time);
    void run();
};

class Polytempo_NetworkEngine : public Polytempo_ScoreSchedulerEngine
{
public:
    void stop();
    void setScoreTime(int time);
    void run();
    
private:
    float waitBeforeStart;
    int   lastDownbeat;
//    Polytempo_Event *currentMarker;
};