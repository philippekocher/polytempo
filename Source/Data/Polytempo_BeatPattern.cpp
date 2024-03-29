#include "JuceHeader.h"
#include "Polytempo_BeatPattern.h"
#include "../Misc/Rational.h"


Polytempo_BeatPattern::Polytempo_BeatPattern()
{}

Polytempo_BeatPattern::~Polytempo_BeatPattern()
{}

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
    
    return length;
}

void Polytempo_BeatPattern::setPattern(String string, bool allowEmptyPattern)
{
    string = string.removeCharacters(" ");
    if(!string.containsOnly("0123456789/+")) return;
    
    Array<Rational *> tempPattern;
    
    StringArray tokens1,tokens2;
    tokens1.addTokens(string, "+", "");
    tokens1.removeEmptyStrings();
    tokens1.trim();
    
    if(tokens1.size() == 1)
    {
        Rational ratio(tokens1[0]);
        for(int i=0;i<ratio.getNumerator();i++)
        {
            tempPattern.add(new Rational(1,ratio.getDenominator()));
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
            if(tokens2[0].getIntValue() > 0)
                tempPattern.insert(0, new Rational(tokens2[0].getIntValue(), denominator, false));
        }
    }
    
    if(tempPattern.size() > 0)
    {
        pattern.clear();
        pattern.addArray(tempPattern);
        patternString = string;
    }
    else if(allowEmptyPattern)
    {
        pattern.clear();
        patternString = "";
    }
}

String Polytempo_BeatPattern::getPattern()
{
    return patternString;
}

StringArray Polytempo_BeatPattern::getPrintableTimeSignature()
{
    StringArray array = StringArray();
    int num = 0, den = 0;
    int tempNum, tempDen;

    for(Rational* p : pattern) {
        tempNum = p->getNumerator();
        tempDen = p->getDenominator();
                
        if(den == 0)
        {
            num = tempNum;
            den = tempDen;
        }
        else if(tempNum == 1 && tempDen == den)
        {
            num += tempNum;
        }
        else
        {
            array.add(String(num));
            array.add(String(den));
            
            num = tempNum;
            den = tempDen;
        }
    }

    array.add(String(num));
    array.add(String(den));

    return array;
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

void Polytempo_BeatPattern::setStartPosition(Rational pos)
{
    startPosition = pos;
}

Rational Polytempo_BeatPattern::getStartPosition()
{
    return startPosition;
}

Array<Polytempo_Event *> Polytempo_BeatPattern::getEvents(Rational pos)
{
    Polytempo_Event* event;
    Array<Polytempo_Event *> beatEvents;
    int counter = currentCounter;
    
    if(counterString != "+")
        counter = currentCounter = counterString.getIntValue();
    
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
                    event->setProperty(eventPropertyString_Value,counter);

                beatEvents.add(event);
                
                counter++;
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
    if(!patternString.isEmpty())
        object->setProperty("patternString", patternString);
    if(repeats > 1)
        object->setProperty("repeats", repeats);
    if(!marker.isEmpty())
        object->setProperty("marker", marker);
    if(!counterString.isEmpty())
        object->setProperty("counterString", counterString);

    return object;
}

void Polytempo_BeatPattern::setObject(DynamicObject* object)
{
    setPattern(object->getProperty("patternString"));
    repeats = object->getProperty("repeats");
    if(repeats < 1) repeats = 1;
    marker = object->getProperty("marker");
    counterString = object->getProperty("counterString");
}


