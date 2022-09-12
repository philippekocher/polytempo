#pragma once

#include "../Misc/Rational.h"
#include "Polytempo_BeatPattern.h"


class Polytempo_ControlPoint
{
public:
    Polytempo_ControlPoint(): time(0)
    {
    }

    ~Polytempo_ControlPoint()
    {}
    
    Polytempo_ControlPoint* copy()
    {
        Polytempo_ControlPoint* temp = new Polytempo_ControlPoint();
        temp->time = time;
        temp->position = position;
        temp->positionString = positionString;
        temp->tempoIn = tempoIn;
        temp->tempoOut = tempoOut;
        temp->tempoInWeight = tempoInWeight;
        temp->tempoOutWeight = tempoOutWeight;
        temp->cueIn.setPattern(cueIn.getPattern());
        temp->start = start;
        temp->locked = locked;
        temp->isCoinciding = isCoinciding;
        
        return temp;
    }
    
    float    time; // in seconds
    Rational position = 0;
    String   positionString;
    float    tempoIn = 0.25f; // 0.25 is crotchet = 60 bpm, better choice?
    float    tempoOut = 0.25f;
    float    tempoInWeight = 0.33f;
    float    tempoOutWeight = 0.33f;
    Polytempo_BeatPattern cueIn;

    bool start = false;
    bool locked = false;
    bool isCoinciding = false;
        
    DynamicObject* getObject()
    {
        DynamicObject* object = new DynamicObject();
        object->setProperty("time", time);
        object->setProperty("position", position.toString());
        object->setProperty("tempoIn", tempoIn);
        object->setProperty("tempoOut", tempoOut);
        object->setProperty("tempoInWeight", tempoInWeight);
        object->setProperty("tempoOutWeight", tempoOutWeight);
        object->setProperty("cueIn", cueIn.getObject());
        object->setProperty("start", start);
        object->setProperty("locked", locked);

        return object;
    }
    
    void setObject(DynamicObject* object)
    {
        time = object->getProperty("time");
        position = Rational((object->getProperty("position")).toString());
        tempoIn = object->getProperty("tempoIn");
        tempoOut = object->getProperty("tempoOut");
        tempoInWeight = object->getProperty("tempoInWeight");
        tempoOutWeight = object->getProperty("tempoOutWeight");
        if(object->hasProperty("cueIn"))
            cueIn.setObject(object->getProperty("cueIn").getDynamicObject());
        start = object->getProperty("start");
        locked = object->getProperty("locked");
    }
};

class ControlPointTimeComparator
{
public:
    static int compareElements(Polytempo_ControlPoint* v1, Polytempo_ControlPoint* v2) throw()
    {
        float t1 = v1->time;
        float t2 = v2->time;
        
        if(t1>t2) return 1;
        if(t1<t2) return -1;
        
        return 0;
    }
};

//--------------------------------


class Polytempo_TempoInterpolation
{
public:
    static Array < Point < float >* > getPoints(int num, Polytempo_ControlPoint* cp1, Polytempo_ControlPoint* cp2);
    static Array < float > getTempos(int num, Polytempo_ControlPoint* cp1, Polytempo_ControlPoint* cp2);
    static float getTime(Rational pos, Polytempo_ControlPoint* cp1, Polytempo_ControlPoint* cp2);
    static Rational getPosition(float time, Polytempo_ControlPoint* cp1, Polytempo_ControlPoint* cp2);
    static float getTempo(Rational pos, Polytempo_ControlPoint* cp1, Polytempo_ControlPoint* cp2);
    static bool validateCurve(Polytempo_ControlPoint* cp1, Polytempo_ControlPoint* cp2);
};
