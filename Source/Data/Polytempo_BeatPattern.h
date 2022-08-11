#pragma once

#include "../Misc/Rational.h"
#include "../Scheduler/Polytempo_Event.h"

/*
 BeatPattern (a kind of "pattern factory" for beat events)
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
    void   setPattern(String, bool allowEmptyPattern = false);
    String getPattern();
    StringArray getPrintableTimeSignature();
    void   setRepeats(int);
    int    getRepeats();
    void   setMarker(String);
    String getMarker();
    void   setCounter(String);
    String getCounter();
    
    // pattern expansion
    void setCurrentCounter(int);
    int getCurrentCounter();
    void setStartPosition(Rational pos);
    Rational getStartPosition();
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
    Rational startPosition;
    
    OwnedArray<Rational> pattern;
};
