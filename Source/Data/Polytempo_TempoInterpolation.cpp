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

#include "Polytempo_TempoInterpolation.h"

void calculateBezierPoints(Polytempo_ControlPoint* cp1, Polytempo_ControlPoint* cp2, Point<float>* p0, Point<float>* p1, Point<float>* p2, Point<float>* p3)
{
    // p0 and p3 correlate with cp1 and cp2
    
    p0->x = cp1->time;
    p0->y = cp1->position.toFloat();
    
    p3->x = cp2->time;
    p3->y = cp2->position.toFloat();
    
    
    // the additional points p1 and p2 have to be calculated
    
    double dist = sqrt(pow(p3->x - p0->x, 2) + pow(p3->y - p0->y, 2));
    double theta1 = atan2(cp1->tempoOut,1), theta2 = atan2(cp2->tempoIn,1);
    
    //double weight = 0.333; // TODO: take tempoInWeight and tempoOutWeight
    
    p1->x = p0->x + float(dist * cp1->tempoOutWeight * cos(theta1));
    p1->y = p0->y + float(dist * cp1->tempoOutWeight * sin(theta1));
    
    p2->x = p3->x - float(dist * cp2->tempoInWeight * cos(theta2));
    p2->y = p3->y - float(dist * cp2->tempoInWeight * sin(theta2));
}

Array < Point < float >* > Polytempo_TempoInterpolation::getPoints(int num, Polytempo_ControlPoint* cp1, Polytempo_ControlPoint* cp2)
{
    Array<Point<float> *> result;
    Point<float>* point;
    Point<float> p0, p1, p2, p3;
    double t;
    
    calculateBezierPoints(cp1, cp2, &p0, &p1, &p2, &p3);
    
    for(int i=0; i<num; i++)
    {
        t = float(i) * 1/(num-1);
        point = new Point<float>();
        
        point->x = float((-1 * p0.x + 3 * p1.x - 3 * p2.x + p3.x) * t * t * t +
        ( 3 * p0.x - 6 * p1.x + 3 * p2.x) * t * t +
        (-3 * p0.x + 3 * p1.x) * t +
        p0.x);
        
        point->y = float((-1 * p0.y + 3 * p1.y - 3 * p2.y + p3.y) * t * t * t +
        ( 3 * p0.y - 6 * p1.y + 3 * p2.y) * t * t +
        (-3 * p0.y + 3 * p1.y) * t +
        p0.y);
        
        result.add(point);
    }
    
    return result;
}

Array < float > Polytempo_TempoInterpolation::getTempos(int num, Polytempo_ControlPoint* cp1, Polytempo_ControlPoint* cp2)
{
    Array < float > result;
    float tempo;
    Point<float> p0, p1, p2, p3;
    float mx, my;
    double t;
    
    calculateBezierPoints(cp1, cp2, &p0, &p1, &p2, &p3);
    
    for(int i=0; i<num; i++)
    {
        t = float(i) * 1/(num-1);
        
        mx = float(3 * (1 - t) * (1 - t) * (p1.x - p0.x) +
        6 * (1 - t) * t * (p2.x - p1.x) +
        3 * t * t * (p3.x - p2.x));
        
        my = float(3 * (1 - t) * (1 - t) * (p1.y - p0.y) +
        6 * (1 - t) * t * (p2.y - p1.y) +
        3 * t * t * (p3.y - p2.y));
        
        tempo = 1/(mx/my);
        
        result.add(tempo);
    }
    
    return result;
}

float Polytempo_TempoInterpolation::getTime(Rational pos, Polytempo_ControlPoint* cp1, Polytempo_ControlPoint* cp2)
{
    Point<float> p0, p1, p2, p3;
    double t0, t, x=0, y, y0 = pos.toFloat(), incr;
    bool done = false;
    const double epsilon = 1e-6;
    
    calculateBezierPoints(cp1, cp2, &p0, &p1, &p2, &p3);
    
    // no approximation needed
    if     (pos == cp1->position) return cp1->time;
    else if(pos == cp2->position) return cp2->time;
    else if(pos > cp2->position)  return NAN;
        
    // approximation
    t0 = 0;
    incr = 0.5;
    int i=0;
    while(!done)
    {
        if(++i == 1000)
        {
            DBG("too many approximation steps ("<<i<<")");
            done = true;
        }
        
        t = t0+incr;
        y = (-1 * p0.y + 3 * p1.y - 3 * p2.y + p3.y) * t * t * t +
            ( 3 * p0.y - 6 * p1.y + 3 * p2.y) * t * t +
            (-3 * p0.y + 3 * p1.y) * t +
        p0.y;
        
        if(fabs(y-y0) <= epsilon)
        {
            // calculate x only when y has been found
            x = (-1 * p0.x + 3 * p1.x - 3 * p2.x + p3.x) * t * t * t +
                ( 3 * p0.x - 6 * p1.x + 3 * p2.x) * t * t +
                (-3 * p0.x + 3 * p1.x) * t +
            p0.x;
            
            done = true;
        }
        else if(y > y0)
        {
            incr *= 0.5;
        }
        else
        {
            t0 = t;
            incr *= 0.5;
        }
    }
    return float(x);
}


float Polytempo_TempoInterpolation::getTempo(Rational pos, Polytempo_ControlPoint* cp1, Polytempo_ControlPoint* cp2)
{
    Point<float> p0, p1, p2, p3;
    double t0, t, y, y0 = pos.toFloat(), incr;
    bool done = false;
    const double epsilon = 1.e-6;
    
    calculateBezierPoints(cp1, cp2, &p0, &p1, &p2, &p3);
    
    // approximation
    float mx, my, tempo=0;
    int i = 0;
    t0 = 0;
    incr = 0.5;
    while(!done)
    {
        if(++i == 1000)
        {
            DBG("too many approximation steps ("<<i<<")");
            done = true;
        }
        
        t = t0+incr;
        y = (-1 * p0.y + 3 * p1.y - 3 * p2.y + p3.y) * t * t * t +
        ( 3 * p0.y - 6 * p1.y + 3 * p2.y) * t * t +
        (-3 * p0.y + 3 * p1.y) * t +
        p0.y;
        
        if (fabs(y-y0) < epsilon)
        {
            // calculate only when x has been found
            mx = float(3 * (1 - t) * (1 - t) * (p1.x - p0.x) +
            6 * (1 - t) * t * (p2.x - p1.x) +
            3 * t * t * (p3.x - p2.x));
            
            my = float(3 * (1 - t) * (1 - t) * (p1.y - p0.y) +
            6 * (1 - t) * t * (p2.y - p1.y) +
            3 * t * t * (p3.y - p2.y));
            
            tempo = 1/(mx/my);
            
            done = true;
        }
        else if(y > y0)
        {
            incr *= 0.5;
        }
        else
        {
            t0 = t;
            incr *= 0.5;
        }
    }
    return tempo;
}

bool Polytempo_TempoInterpolation::validateCurve(Polytempo_ControlPoint *cp1, Polytempo_ControlPoint *cp2)
{
    Point<float> p0, p1, p2, p3;
    float mx, my;
    double t;
    int num = 10; // is this enough??
    
    calculateBezierPoints(cp1, cp2, &p0, &p1, &p2, &p3);
    
    for(int i=0; i!=num; i++)
    {
        t = float(i) * 1/num;

        mx = float(3 * (1 - t) * (1 - t) * (p1.x - p0.x) +
        6 * (1 - t) * t * (p2.x - p1.x) +
        3 * t * t * (p3.x - p2.x));
        
        my = float(3 * (1 - t) * (1 - t) * (p1.y - p0.y) +
        6 * (1 - t) * t * (p2.y - p1.y) +
        3 * t * t * (p3.y - p2.y));
        
        if(mx*my <= 0) return false;
    }
    
    return true;
}
