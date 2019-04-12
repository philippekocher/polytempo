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

#include "../../JuceLibraryCode/JuceHeader.h"
#include "Polytempo_BeatPattern.h"
#include "../Misc/Rational.h"


Polytempo_BeatPattern::Polytempo_BeatPattern()
{}

Polytempo_BeatPattern::~Polytempo_BeatPattern()
{};

//------------------------------------------------------------------------------
#pragma mark -
#pragma mark accessors

Rational Polytempo_BeatPattern::getLength()
{
    Rational length = 0;
    
    for(int i=0;i<pattern.size();i++)
    {
        length = length + *(pattern[i]);
    }
    
    return length * repeats;
}

void Polytempo_BeatPattern::setPattern(String string)
{
    pattern.clear();
    patternString = string;
    
    StringArray tokens1,tokens2;
    tokens1.addTokens(patternString, "+", "");
    tokens1.removeEmptyStrings();
    tokens1.trim();
    
    if(tokens1.size() == 1)
    {
        Rational ratio(tokens1[0]);
        for(int i=0;i<ratio.getNumerator();i++)
        {
            pattern.add(new Rational(1,ratio.getDenominator()));
        }
    }
    else
    {
        int denominator = 4;

        for(int i=tokens1.size()-1;i>=0;i--)
        {
            tokens2.clear();
            tokens2.addTokens(tokens1[i], "/", "");
            tokens2.removeEmptyStrings();
            tokens2.trim();
            
            if(tokens2.size() > 1)
            {
                denominator = tokens2[1].getIntValue();
            }
            pattern.insert(0, new Rational(tokens2[0].getIntValue(), denominator));
        }
    }
}

String Polytempo_BeatPattern::getPattern()
{
    return patternString;
}

void Polytempo_BeatPattern::setRepeats(int n)
{
    repeats = n > 0 ? n : 1;
}

int Polytempo_BeatPattern::getRepeats()
{
    return repeats;
}

void Polytempo_BeatPattern::setMarker(String string)
{
    marker = string;
}

String Polytempo_BeatPattern::getMarker()
{
    return marker;
}

void Polytempo_BeatPattern::setCounter(String string)
{
    if(!string.containsOnly("0123456789.+") || string.isEmpty())
        counterString = "+";
    else
        counterString = string;
}

String Polytempo_BeatPattern::getCounter()
{
    return counterString;
}



//------------------------------------------------------------------------------
#pragma mark -
#pragma mark pattern expansion

void Polytempo_BeatPattern::setCurrentCounter(int n)
{
    currentCounter = n;
}

int Polytempo_BeatPattern::getCurrentCounter()
{
    return currentCounter;
}

Array<Polytempo_Event *> Polytempo_BeatPattern::getEvents(Rational pos)
{
    Polytempo_Event* event;
    Array<Polytempo_Event *> beatEvents;
    
    if(counterString != "+")
        currentCounter = counterString.getIntValue();
    
    // marker events
    if(!marker.isEmpty())
    {
        event = new Polytempo_Event(eventType_Marker);
        event->setOwned(true);
        event->setPosition(pos);
        event->setProperty(eventPropertyString_Value, marker);
        
        beatEvents.add(event);
    }
    
    for(int i=0;i<repeats;i++)
    {
        for(int j=0;j<pattern.size();j++)
        {
            // beat events
            event = new Polytempo_Event(eventType_Beat);
            event->setOwned(true);
            event->setPosition(pos);
            event->setProperty(eventPropertyString_Value, pattern[j]->toFloat());
            
            if(pattern.size() == 1)
                event->setProperty(eventPropertyString_Pattern, 11);
            else if(j == 0)
                event->setProperty(eventPropertyString_Pattern, 12);
            else if(j == pattern.size() - 1)
                event->setProperty(eventPropertyString_Pattern, 21);
            else
                event->setProperty(eventPropertyString_Pattern, 22);
            
            beatEvents.add(event);
            
            // counter events
            if(int(event->getProperty(eventPropertyString_Pattern)) < 20)
            {
                event = new Polytempo_Event(eventType_Marker);
                event->setOwned(true);
                event->setPosition(pos);
                if(i == 0 && counterString != "+")
                    event->setProperty(eventPropertyString_Value, counterString);
                else
                    event->setProperty(eventPropertyString_Value,currentCounter);

                beatEvents.add(event);
                
                currentCounter++;
            }
            
            pos = pos + *(pattern[j]);
        }
    }

    return beatEvents;
}

//------------------------------------------------------------------------------
#pragma mark -
#pragma mark serialisation

DynamicObject* Polytempo_BeatPattern::getObject()
{
    DynamicObject* object = new DynamicObject();
    object->setProperty("patternString", patternString);
    object->setProperty("repeats", repeats);
    if(marker != String()) object->setProperty("marker", marker);
    object->setProperty("counterString", counterString);

    return object;
}

void Polytempo_BeatPattern::setObject(DynamicObject* object)
{
    setPattern(object->getProperty("patternString"));
    repeats = object->getProperty("repeats");
    marker = object->getProperty("marker");
    counterString = object->getProperty("counterString");
}


