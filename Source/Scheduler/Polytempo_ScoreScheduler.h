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

#ifndef __Polytempo_ScoreScheduler__
#define __Polytempo_ScoreScheduler__

#include "../Data/Polytempo_Score.h"
#include "Polytempo_ScoreSchedulerEngine.h"
#include "../Scheduler/Polytempo_EventObserver.h"


class Polytempo_ScoreSchedulerEngine;

class Polytempo_ScoreScheduler : public Polytempo_EventObserver
{
public:
    Polytempo_ScoreScheduler();
    ~Polytempo_ScoreScheduler();
    
    juce_DeclareSingleton(Polytempo_ScoreScheduler, false);
    
    void setEngine(Polytempo_ScoreSchedulerEngine*);
    bool isRunning();
    
    void eventNotification(Polytempo_Event *event);

    /* playback
     --------------------------------------- */
    void startStop();
    void start();
    void stop();
    void pause(Polytempo_Event*);
    void kill();
    void returnToLocator();
    void returnToBeginning();
    bool gotoMarker(Polytempo_Event *event, bool storeLocator = true);
    bool gotoMarker(String marker, bool storeLocator = true);
    void skipToEvent(Polytempo_EventType type, bool backwards = false);
    void gotoTime(Polytempo_Event *event);
    void gotoTime(int time);
    void storeLocator(int loc);
    void setTempoFactor(Polytempo_Event *event);
    void executeInit();
    
    /* accessors
     --------------------------------------- */
    void setScore(Polytempo_Score* score_);
    int getScoreTime();

private:
    ScopedPointer < Polytempo_ScoreSchedulerEngine > engine;
    Array < class Polytempo_EventObserver * > observers;
    
    int storedLocator = 0; // milliseconds
    
    Polytempo_Score *score = nullptr;
};


#endif // __Polytempo_ScoreScheduler__
