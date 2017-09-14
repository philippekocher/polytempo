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

#ifndef __Polytempo_Score__
#define __Polytempo_Score__

#include "../Scheduler/Polytempo_Event.h"


class Polytempo_Score_Section;

class Polytempo_Score
{
public:
    Polytempo_Score();
    ~Polytempo_Score();
    
    void setDirty(bool flag = true);
    bool isDirty();
    
    void clear(bool clearInit = false);

    void addEvent(Polytempo_Event*, bool addToInit = false);
    void addEvents(OwnedArray < Polytempo_Event >&);
    void removeEvent(Polytempo_Event*, bool removeFromInit = false);
    
    void setSection(String sectionName = String::empty);
    void sortSection(int sectionIndex = -1);
    
    void setTime(int time);
    bool setTime(int time, Array<Polytempo_Event*> *events, float *waitBeforStart);

    Polytempo_Event* searchEvent(int referenceTime, Polytempo_EventType = eventType_None, bool searchBackwards = false);
    Polytempo_Event* getNextEvent();
    Polytempo_Event* getFirstEvent();
    bool getTimeForMarker(String marker, int *time);
    bool getMarkerForTime(int time, String* marker);
    
    Polytempo_Score_Section *getInitSection();
    OwnedArray < Polytempo_Event >* getInitEvents();
    Array < Polytempo_Event* > getEvents(Polytempo_EventType);

    static void parse(File& file, Polytempo_Score** score);
    
    void writeToFile(File&);

private:
    static void parseJSON(File& JSONFile, Polytempo_Score** scoreFile);
    //static void parseXML(File& XMLFile, Polytempo_Score** scoreFile);
    
    ScopedPointer < StringArray > sectionMap;
    OwnedArray < Polytempo_Score_Section > sections;
    ScopedPointer < Polytempo_Score_Section > initSection;
    
    int    currentSectionIndex;
    uint32 nextEventIndex;
    
    bool dirty = false;
};

#endif // __Polytempo_Score__
