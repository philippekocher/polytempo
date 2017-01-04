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

#ifndef __Polytempo_Scheduler__
#define __Polytempo_Scheduler__

//#include "../../JuceLibraryCode/JuceHeader.h"
#include "../Data/Polytempo_Score.h"
#include "Polytempo_EventObserver.h"
#include "Polytempo_SchedulerEngine.h"

class Polytempo_SchedulerEngine;

class Polytempo_Scheduler
{
public:
    Polytempo_Scheduler();
    ~Polytempo_Scheduler();
    
    juce_DeclareSingleton (Polytempo_Scheduler, false);
    
    void setEngine(Polytempo_SchedulerEngine*);
    bool isRunning();
    
    /* network events
     --------------------------------------- */
    void handleEvent(Polytempo_Event *event, int deferTime = 0);

    /* notify event observers
     --------------------------------------- */
    void registerObserver(Polytempo_EventObserver *obs);
    void removeObserver(Polytempo_EventObserver *obs);
    void notify(Polytempo_Event* event);
        
    /* playback
     --------------------------------------- */
    void startStop();
    void start();
    void stop();
    void pause(Polytempo_Event*);
    void returnToLocator();
    void returnToBeginning();
    bool gotoMarker(Polytempo_Event *event, bool storeLocator = true);
    void skipToEvent(Polytempo_EventType type, bool backwards = false);
    void gotoLocator(Polytempo_Event *event);
    void storeLocator(float loc);
    void setTempoFactor(Polytempo_Event *event);
    
    /* accessors
     --------------------------------------- */
    void setScore(Polytempo_Score* score_);

private:
    ScopedPointer<Polytempo_SchedulerEngine> engine;
    Array < class Polytempo_EventObserver * > observers;
    
    float storedLocator;
    
    Polytempo_Score *score = nullptr;
    Polytempo_Score *tempScore; // to hold scheduled messages

    //int  realTime;
    int  scoreTimeOffset, realTimeOffset;
};


#endif // __Polytempo_Scheduler__
