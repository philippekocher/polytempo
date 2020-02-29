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

#ifndef __Polytempo_TempoInterpolation__
#define __Polytempo_TempoInterpolation__

#include "../Misc/Rational.h"


class Polytempo_ControlPoint
{
public:
    Polytempo_ControlPoint()
    {}
    
    ~Polytempo_ControlPoint()
    {}
    
    Polytempo_ControlPoint* copy()
    {
        Polytempo_ControlPoint* temp = new Polytempo_ControlPoint();
        temp->time = time;
        temp->position = position;
        
        return temp;
    }
    
    float    time; // in seconds
    Rational position = 0;
    float    tempoIn = 0.25f; // 0.25 is crotchet = MM 60, better choice?
    float    tempoOut = 0.25f;
    float    tempoInWeight = 0.33f;
    float    tempoOutWeight = 0.33f;
    
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
    static float getTempo(Rational pos, Polytempo_ControlPoint* cp1, Polytempo_ControlPoint* cp2);
    static bool validateCurve(Polytempo_ControlPoint* cp1, Polytempo_ControlPoint* cp2);
};



#endif  // __Polytempo_TempoInterpolation__
