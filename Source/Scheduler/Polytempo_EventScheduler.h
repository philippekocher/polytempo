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

#ifndef __Polytempo_EventScheduler__
#define __Polytempo_EventScheduler__

#include "Polytempo_EventObserver.h"


class Polytempo_EventScheduler : public Thread
{
public:
    Polytempo_EventScheduler();
    ~Polytempo_EventScheduler();
    
    juce_DeclareSingleton(Polytempo_EventScheduler, false);
    
    /* schedule events
     --------------------------------------- */

    void scheduleScoreEvent(Polytempo_Event *event, bool useCopy = false);
    void deletePendingScoreEvents();

    void scheduleEvent(Polytempo_Event *event);

    /* thread
     --------------------------------------- */
    void run();

    /* notify event observers
     --------------------------------------- */
    void registerObserver(Polytempo_EventObserver *obs);
    void removeObserver(Polytempo_EventObserver *obs);
private:
    void notify(Polytempo_Event* event);
    
    Array < class Polytempo_EventObserver * > observers;
    OwnedArray < class Polytempo_Event > scheduledScoreEvents;
    int currentSyncTime;
    bool deleteScoreEvents = false;

};


#endif // __Polytempo_EventScheduler__
