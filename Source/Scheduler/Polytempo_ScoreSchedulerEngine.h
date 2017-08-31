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

#ifndef __Polytempo_ScoreSchedulerEngine__
#define __Polytempo_ScoreSchedulerEngine__


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
    float tempoFactor = 1;
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
    float lastDownbeat;
//    Polytempo_Event *currentMarker;
};


#endif  // __Polytempo_ScoreSchedulerEngine__
