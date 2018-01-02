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

#ifndef __Polytempo_BeatPattern__
#define __Polytempo_BeatPattern__

#include "../Misc/Rational.h"
#include "../Scheduler/Polytempo_Event.h"

/*
 BeatPattern (a kind of "pattern factory" for beat events)
 — score position (= starting point, if repeated)
 — pattern (a metre or a single value to be repeated)
 — repeats
 — marker (a rehearsal letter at the beginning of the pattern)
 - counter (the first bar number, incremented at every downbeat)
 */

class Polytempo_BeatPattern
{
public:
    Polytempo_BeatPattern();
    ~Polytempo_BeatPattern();
    
    // accessors
    Rational getLength();
    void   setPattern(String);
    String getPattern();
    void   setRepeats(int);
    int    getRepeats();
    void   setMarker(String);
    String getMarker();
    void   setCounter(String);
    String getCounter();
    
    // pattern expansion
    void setCurrentCounter(int);
    int getCurrentCounter();
    Array<Polytempo_Event *> getEvents(Rational pos);
    
    // serialisation
    DynamicObject* getObject();
    void setObject(DynamicObject* object);

private:
    String patternString;
    int    repeats = 1;
    String marker;
    String counterString;
    int    currentCounter = 0;
    
    OwnedArray<Rational> pattern;
};



#endif  // __Polytempo_BeatPattern__
