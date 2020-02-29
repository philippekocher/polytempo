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

#include "Polytempo_Sequence.h"
#include "Polytempo_Composition.h"
#include "../Views/PolytempoComposer/Polytempo_ListComponent.h"
#include "../Preferences/Polytempo_StoredPreferences.h"


Polytempo_Sequence::Polytempo_Sequence()
{}

Polytempo_Sequence::~Polytempo_Sequence()
{}


//------------------------------------------------------------------------------
#pragma mark -
#pragma mark accessors

String Polytempo_Sequence::getName()
{
    return name;
}

Colour Polytempo_Sequence::getColour()
{
    return colour;

}

OwnedArray <Polytempo_ControlPoint>* Polytempo_Sequence::getControlPoints()
{
    return &controlPoints;
}

Polytempo_ControlPoint* Polytempo_Sequence::getControlPoint(int index)
{
    if(index < 0 || index >= controlPoints.size())
        return nullptr;
    else
        return controlPoints[index];
}

OwnedArray <Polytempo_BeatPattern>* Polytempo_Sequence::getBeatPatterns()
{
    return &beatPatterns;
}

Polytempo_BeatPattern* Polytempo_Sequence::getBeatPattern(int index)
{
    if(index < 0 || index >= beatPatterns.size())
        return nullptr;
    else
        return beatPatterns[index];
}

OwnedArray <Polytempo_Event>& Polytempo_Sequence::getEvents()
{
    return events;
}

Polytempo_Event* Polytempo_Sequence::getEvent(int index)
{
    if(index < 0 || index >= events.size())
        return nullptr;
    else
        return events[index];
}

bool Polytempo_Sequence::isVisible()
{
    return visible;
}


void Polytempo_Sequence::setName(String s)
{
    if(name != s)
    {
        name = s;
        Polytempo_Composition::getInstance()->setDirty(true);
    }
}

void Polytempo_Sequence::setIndex(int index)
{
    sequenceIndex = index;
    Polytempo_Composition::getInstance()->setDirty(true);
}

void Polytempo_Sequence::setColour(Colour c)
{
    colour = c;
    Polytempo_Composition::getInstance()->setDirty(true);
}

void Polytempo_Sequence::setVisibility(bool value)
{
    visible = value;
    Polytempo_Composition::getInstance()->setDirty(true);
}

void Polytempo_Sequence::setBeatPatternListComponent(Polytempo_ListComponent* component)
{
    beatPatternListComponent = component;
}


bool Polytempo_Sequence::validateNewControlPointPosition(float t, Rational pos)
{
    // check if a new control point is valid before it is added
    
    // a new point or a point at the end
    if(controlPoints.size() == 0 ||
       (t > controlPoints.getLast()->time &&
       pos > controlPoints.getLast()->position))
        return true;

    // a second point
    if(controlPoints.size() == 1 &&
       ((t >= controlPoints[0]->time && pos > controlPoints[0]->position) ||
       (t <= controlPoints[0]->time && pos < controlPoints[0]->position)))
        return true;

    // a point between two existing points
    for(int i=0;i<controlPoints.size()-1;i++)
    {
        if(t >= controlPoints[i]->time &&
           t <= controlPoints[i+1]->time &&
           pos > controlPoints[i]->position &&
           pos < controlPoints[i+1]->position)
            
           return true;
    }
    
    return false;
}

bool Polytempo_Sequence::validateControlPoints(OwnedArray<Polytempo_ControlPoint> &points)
{
    for(int i=0;i<points.size();i++)
    {
        if(i > 0 && points[i]->time     <= points[i-1]->time)     return false;
        if(i > 0 && points[i]->position <= points[i-1]->position) return false;
        if(i < points.size()-1 && points[i]->time     >  points[i+1]->time)     return false;
        if(i < points.size()-1 && points[i]->position >= points[i+1]->position) return false;
    }
    return true;
}

void Polytempo_Sequence::setControlPointPosition(int index, float t, Rational pos)
{
    // negative values are ignored
    if(t   < 0) t   = controlPoints[index]->time;
    if(pos < 0) pos = controlPoints[index]->position;
    
    // validate time and position
    if((index > 0 && t   <= controlPoints[index-1]->time) ||
       (index > 0 && pos <= controlPoints[index-1]->position) ||
       (index < controlPoints.size()-1 && t   >  controlPoints[index+1]->time) ||
       (index < controlPoints.size()-1 && pos >= controlPoints[index+1]->position)) return;

    controlPoints[index]->time = t;
    controlPoints[index]->position = pos;
    updateEvents();

    Polytempo_Composition::getInstance()->updateContent();
    Polytempo_Composition::getInstance()->setDirty(true);
}

void Polytempo_Sequence::setControlPointStartAndCue(int index, int start, int cue)
{
    if(start >= 0) controlPoints[index]->start = start != 0;
    if(cue >= 0)   controlPoints[index]->cueIn = cue;
    
    updateEvents();

    Polytempo_Composition::getInstance()->updateContent();
    Polytempo_Composition::getInstance()->setDirty(true);
}

void Polytempo_Sequence::shiftControlPoints(Array<int>* indices, float deltaTime)
{
    // tentative
    OwnedArray<Polytempo_ControlPoint> tempControlPoints;
    for(Polytempo_ControlPoint* point : controlPoints) tempControlPoints.add(point->copy());
    
    for(int index : *indices)
    {
        tempControlPoints[index]->time += deltaTime;
    }
    
    if(!validateControlPoints(tempControlPoints))
    {
        Polytempo_Alert::show("Error", "Invalid operation");
        return;
    }

    for(int index : *indices)
    {
        controlPoints[index]->time += deltaTime;
    }
    updateEvents();
    Polytempo_Composition::getInstance()->updateContent();
    Polytempo_Composition::getInstance()->setDirty(true);
}


void Polytempo_Sequence::setControlPointTempos(int index, float inTempo, float outTempo, float inTempoWeight, float outTempoWeight)
{
    if(inTempo > 0)  controlPoints[index]->tempoIn  = inTempo;
    if(outTempo > 0) controlPoints[index]->tempoOut = outTempo;
    if(inTempoWeight >= 0)   controlPoints[index]->tempoInWeight = inTempoWeight;
    if(outTempoWeight >= 0)  controlPoints[index]->tempoOutWeight = outTempoWeight;

    updateEvents();

    Polytempo_Composition::getInstance()->setDirty(true);
}

bool Polytempo_Sequence::isTempoConstantAfterPoint(int i)
{
    if(i == controlPoints.size() - 1) return false;
    
    else if(controlPoints[i]->tempoOut == (controlPoints[i+1]->position - controlPoints[i]->position).toFloat() / (controlPoints[i+1]->time - controlPoints[i]->time) &&
            controlPoints[i]->tempoOut == controlPoints[i+1]->tempoIn)
        return true;
    else
        return false;
}

void Polytempo_Sequence::adjustTime(Array<int>* indices)
{
    Polytempo_ControlPoint *c0, *c1;
    float meanTempo;
    
    if(indices->getUnchecked(0) == 0) indices->remove(0); // first point can't be adjusted
    
    // tentative
    OwnedArray<Polytempo_ControlPoint> tempControlPoints;
    for(Polytempo_ControlPoint* point : controlPoints) tempControlPoints.add(point->copy());
    
    for(int index : *indices)
    {
        c0 = tempControlPoints[index-1];
        c1 = tempControlPoints[index];
        meanTempo = (c0->tempoOut + c1->tempoIn) * 0.5f;
        c1->time = c0->time + (c1->position - c0->position).toFloat() / meanTempo;
    }
    
    if(!validateControlPoints(tempControlPoints))
    {
        Polytempo_Alert::show("Error", "Invalid operation");
        return;
    }

    for(int index : *indices)
    {
        c0 = controlPoints[index-1];
        c1 = controlPoints[index];
        meanTempo = (c0->tempoOut + c1->tempoIn) * 0.5f;
        c1->time = c0->time + (c1->position - c0->position).toFloat() / meanTempo;
    }
    updateEvents();
    Polytempo_Composition::getInstance()->updateContent();
    Polytempo_Composition::getInstance()->setDirty(true);
}

void Polytempo_Sequence::adjustPosition(Array<int>* indices)
{
    Polytempo_ControlPoint *c0, *c1;
    float meanTempo;

    if(indices->getUnchecked(0) == 0) indices->remove(0); // first point can't be adjusted

    // tentative
    OwnedArray<Polytempo_ControlPoint> tempControlPoints;
    for(Polytempo_ControlPoint* point : controlPoints) tempControlPoints.add(point->copy());
    
    for(int index : *indices)
    {
        c0 = tempControlPoints[index-1];
        c1 = tempControlPoints[index];
        meanTempo = (c0->tempoOut + c1->tempoIn) * 0.5f;
        c1->position = Rational(c0->position.toFloat() + (c1->time - c0->time) * meanTempo);
    }
    
    if(!validateControlPoints(tempControlPoints))
    {
        Polytempo_Alert::show("Error", "Invalid operation");
        return;
    }

    for(int index : *indices)
    {
        c0 = controlPoints[index-1];
        c1 = controlPoints[index];
        meanTempo = (c0->tempoOut + c1->tempoIn) * 0.5f;
        c1->position = Rational(c0->position.toFloat() + (c1->time - c0->time) * meanTempo);
    }
    updateEvents();
    Polytempo_Composition::getInstance()->updateContent();
    Polytempo_Composition::getInstance()->setDirty(true);
}

void Polytempo_Sequence::adjustTempo(Array<int>* indices)
{
    if(indices->getUnchecked(0) == 0) indices->remove(0); // first point can't be adjusted

    for(int index : *indices)
    {
        Polytempo_ControlPoint *c0 = controlPoints[index-1];
        Polytempo_ControlPoint *c1 = controlPoints[index];
        float tempo = (c1->position - c0->position).toFloat() / (c1->time - c0->time);
    
        c0->tempoOut = tempo;
        c1->tempoIn = tempo;
    }
    updateEvents();
    Polytempo_Composition::getInstance()->updateContent();
    Polytempo_Composition::getInstance()->setDirty(true);
}

void Polytempo_Sequence::removeControlPoints(Array<int>* indices)
{
    for(int i=indices->size()-1;i>=0;i--)
        controlPoints.remove(indices->getUnchecked(i));

    updateEvents();
    Polytempo_Composition::getInstance()->updateContent();
    Polytempo_Composition::getInstance()->setDirty(true);
}

void Polytempo_Sequence::addControlPoint(float t, Rational pos, float tin, float tout)
{
    Polytempo_ControlPoint* point = new Polytempo_ControlPoint();
    
    point->position = pos;
    point->time = t;
    
    controlPoints.add(point);
    ControlPointTimeComparator sorter;
    controlPoints.sort(sorter, true);
    
    int index = controlPoints.indexOf(point);
    if(index > 0 && index < controlPoints.size() - 1)
    {
        point->tempoIn  = tin == 0 ? controlPoints[index-1]->tempoOut : tin;
        point->tempoOut = tout == 0 ? controlPoints[index+1]->tempoIn : tout;
    }
    else
    {
        point->tempoIn = 0.25;
        point->tempoOut = 0.25;
    }
    updateEvents();
    Polytempo_Composition::getInstance()->clearSelectedControlPointIndices();
    Polytempo_Composition::getInstance()->addSelectedControlPointIndex(index);
    Polytempo_Composition::getInstance()->setDirty(true);
}

int Polytempo_Sequence::getSelectedBeatPattern()
{
    return selectedBeatPattern;
}

void Polytempo_Sequence::setSelectedBeatPattern(int index)
{
    selectedBeatPattern = index;
}

void Polytempo_Sequence::addBeatPattern()
{
    String defaultBeatPattern = Polytempo_StoredPreferences::getInstance()->getProps().getValue("defaultBeatPattern");

    StringArray tokens;
    tokens.addTokens(defaultBeatPattern, false);
    
    insertBeatPatternAtIndex(beatPatterns.size(), tokens[0], tokens[1].getIntValue(), "+", "");
}

void Polytempo_Sequence::insertBeatPattern()
{
    String defaultBeatPattern = Polytempo_StoredPreferences::getInstance()->getProps().getValue("defaultBeatPattern");
    
    StringArray tokens;
    tokens.addTokens(defaultBeatPattern, false);

    insertBeatPatternAtIndex(selectedBeatPattern, tokens[0], tokens[1].getIntValue(), "+", "");
}

void Polytempo_Sequence::insertBeatPatternAtIndex(int index, const String& pattern, int repeats, const String& counter, const String& marker)
{
    Polytempo_BeatPattern* bp = new Polytempo_BeatPattern();
    bp->setPattern(pattern);
    bp->setRepeats(repeats);
    bp->setCounter(counter);
    bp->setMarker(marker);
    beatPatterns.insert(index, bp);
    
    buildBeatPattern();
    if(beatPatternListComponent != nullptr)
        beatPatternListComponent->setSelection(index);

    Polytempo_Composition::getInstance()->setDirty(true);
}

void Polytempo_Sequence::removeSelectedBeatPattern()
{
    beatPatterns.remove(selectedBeatPattern);
    
    buildBeatPattern();
    Polytempo_Composition::getInstance()->setDirty(true);
}

void Polytempo_Sequence::buildBeatPattern()
{
    events.clear();
    
    if(beatPatterns.size() == 0)
    {
        controlPoints.clear();
        Polytempo_Composition::getInstance()->updateContent();
        return;
    }
    if(controlPoints.size() == 0)
    {
        addControlPoint(0,0);   // start point
        addControlPoint(4,1);   // end point
    }
    
    Rational position = 0;
    int currentCounter = 1;
    
    for(int i=0;i<beatPatterns.size();i++)
    {
        beatPatterns[i]->setCurrentCounter(currentCounter);
        events.addArray(beatPatterns[i]->getEvents(position));
        currentCounter = beatPatterns[i]->getCurrentCounter();

        position = position + beatPatterns[i]->getLength();
    }

    // add a very last event
    Polytempo_Event *event = new Polytempo_Event(eventType_Beat);
    event->setOwned(true);
    event->setPosition(position);
    event->setProperty(eventPropertyString_Pattern, 10);
    event->setValue(events.getLast()->getValue());
    events.add(event);
    
    Polytempo_Composition::getInstance()->updateContent();
    Polytempo_Composition::getInstance()->setDirty(true);
}

void Polytempo_Sequence::updateEvents()
{
    // verify that the first control point is always a start point
    if(controlPoints.size() > 1) controlPoints[0]->start = true;
    
    int cpIndex = -1;
    Polytempo_Event *lastBeatEvent = nullptr;
    
    for(int i=0;i<events.size();i++)
    {
        Polytempo_Event *event = events[i];
        
       if(controlPoints[cpIndex+1] != nullptr &&
          event->getPosition() >= controlPoints[cpIndex+1]->position &&
          controlPoints.size() > cpIndex + 2)
          cpIndex++;
                
        Polytempo_ControlPoint *cp1 = controlPoints[cpIndex];
        Polytempo_ControlPoint *cp2 = controlPoints[cpIndex+1];
        
        if(cp1 == nullptr || cp2 == nullptr || cp2->start)
            event->setProperty(eventPropertyString_Time, NAN);
        else if(Polytempo_TempoInterpolation::validateCurve(cp1,cp2))
            event->setProperty(eventPropertyString_Time, Polytempo_TempoInterpolation::getTime(event->getPosition(), cp1, cp2));
        else
            event->setProperty(eventPropertyString_Time, NAN);

        event->setProperty("~sequence", sequenceIndex);

        if(event->getType() == eventType_Beat)
        {
            if(lastBeatEvent != nullptr)
            {
                lastBeatEvent->setProperty(eventPropertyString_Duration,
                                           float(event->getProperty(eventPropertyString_Time)) -
                                           float(lastBeatEvent->getProperty(eventPropertyString_Time)));
            }
            lastBeatEvent = event;
        }
    }
    if(lastBeatEvent != nullptr)
        lastBeatEvent->setProperty(eventPropertyString_Duration, 1.0f);

}

void Polytempo_Sequence::addPlaybackPropertiesToEvent(Polytempo_Event* event)
{
    int pattern = event->getProperty(eventPropertyString_Pattern);
    
    if(!mute && playAudioClick)
    {
        if(int(event->getProperty("cue")) != 0)
        {
            event->setProperty("audioPitch", audioCuePitch);
            event->setProperty("audioVolume", audioCueVolume);
        }
        else
        {
            if(pattern < 20)
            {
                event->setProperty("audioPitch", audioDownbeatPitch);
                event->setProperty("audioVolume", audioDownbeatVolume);
            }
            else
            {
                event->setProperty("audioPitch", audioBeatPitch);
                event->setProperty("audioVolume", audioBeatVolume);
            }
        }
        event->setProperty("audioChannel", audioChannel);
    }
    if(!mute && playMidiClick)
    {
        if(int(event->getProperty("cue")) != 0)
        {
            event->setProperty("midiPitch", midiCuePitch);
            event->setProperty("midiVelocity", midiCueVelocity);
        }
        else
        {
            if(pattern < 20)
            {
                event->setProperty("midiPitch", midiDownbeatPitch);
                event->setProperty("midiVelocity", midiDownbeatVelocity);
            }
            else
            {
                event->setProperty("midiPitch", midiBeatPitch);
                event->setProperty("midiVelocity", midiBeatVelocity
                                   );
            }
        }
        event->setProperty("midiChannel", midiChannel);
    }
}

Polytempo_Event* Polytempo_Sequence::getOscEvent(Polytempo_Event* event)
{
    if(!mute && sendOsc)
    {
        Polytempo_Event* oscEvent = new Polytempo_Event(eventType_Osc);

        String oscString;
    
        if(int(event->getProperty("cue")) != 0)
        {
            oscString = oscCueMessage;
        }
        else
        {
            int pattern = event->getProperty(eventPropertyString_Pattern);
            
            if(pattern < 20)
            {
                oscString = oscDownbeatMessage;
            }
            else
            {
                oscString = oscBeatMessage;
            }
        }

        oscString = oscString.replace("#pattern", event->getProperty(eventPropertyString_Pattern).toString());
        oscString = oscString.replace("#duration", event->getProperty(eventPropertyString_Duration).toString());
        
        
        StringArray tokens;
        tokens.addTokens(oscString, " ", "");
        tokens.removeEmptyStrings();
        tokens.trim();
        
        oscEvent->setProperty("address", tokens[0]);
        oscEvent->setProperty("senderID", "sequence"+String(sequenceIndex));
        
        var message;
        for(int i=1;i<tokens.size();i++) message.append(tokens[i]);
        
        oscEvent->setProperty("message", message);
        
        oscEvent->setSyncTime(event->getSyncTime());
        
        return oscEvent;
    }
    
    return nullptr;
}

//------------------------------------------------------------------------------
#pragma mark -
#pragma mark serialisation

DynamicObject* Polytempo_Sequence::getObject()
{
    DynamicObject* object = new DynamicObject();
    
    var varControlPoints;
    for(Polytempo_ControlPoint *point : controlPoints)
        varControlPoints.append(point->getObject());
    object->setProperty("controlPoints", varControlPoints);
    
    var varBeatPatterns;
    for(Polytempo_BeatPattern *beatPattern : beatPatterns)
        varBeatPatterns.append(beatPattern->getObject());
    object->setProperty("beatPatterns", varBeatPatterns);
    
    object->setProperty("name", name);
    object->setProperty("colour", colour.toString());
    object->setProperty("mute", mute);

    object->setProperty("playAudioClick", playAudioClick);
    object->setProperty("audioDownbeatPitch", audioDownbeatPitch);
    object->setProperty("audioDownbeatVolume", audioDownbeatVolume);
    object->setProperty("audioBeatPitch", audioBeatPitch);
    object->setProperty("audioBeatVolume", audioBeatVolume);
    object->setProperty("audioCuePitch", audioCuePitch);
    object->setProperty("audioCueVolume", audioCueVolume);
    object->setProperty("audioChannel", audioChannel);
    
    object->setProperty("playMidiClick", playMidiClick);
    object->setProperty("midiDownbeatPitch", midiDownbeatPitch);
    object->setProperty("midiDownbeatVelocity", midiDownbeatVelocity);
    object->setProperty("midiBeatPitch", midiBeatPitch);
    object->setProperty("midiBeatVelocity", midiBeatVelocity);
    object->setProperty("midiCuePitch", midiCuePitch);
    object->setProperty("midiCueVelocity", midiCueVelocity);
    object->setProperty("midiChannel", midiChannel);
    
    object->setProperty("sendOsc", sendOsc);
    object->setProperty("oscDownbeatMessage", oscDownbeatMessage);
    object->setProperty("oscBeatMessage", oscBeatMessage);
    object->setProperty("oscCueMessage", oscCueMessage);
    object->setProperty("oscReceiver", oscReceiver);
    object->setProperty("oscPort", oscPort);
    
    object->setProperty("showName", showName);
    object->setProperty("staveOffset", staveOffset);
    object->setProperty("numberOfStaves", numberOfStaves);
    object->setProperty("secondaryStaveOffset", secondaryStaveOffset);
    object->setProperty("lineOffset", lineOffset);
    object->setProperty("numberOfLines", numberOfLines);
    
    return object;
}

void Polytempo_Sequence::setObject(DynamicObject* object)
{
    name = object->getProperty("name");
    colour = Colour::fromString(object->getProperty("colour").toString());
    
    beatPatterns.clear();
    if(!object->getProperty("beatPatterns")) return;
    for(var varBeatPattern : *object->getProperty("beatPatterns").getArray())
    {
        Polytempo_BeatPattern* bp = new Polytempo_BeatPattern();
        bp->setObject(varBeatPattern.getDynamicObject());
        beatPatterns.add(bp);
    }
    if(beatPatterns.size() == 0) addBeatPattern();
    buildBeatPattern();
    
    controlPoints.clear();
    for(var varControlPoint : *object->getProperty("controlPoints").getArray())
    {
        Polytempo_ControlPoint *cp = new Polytempo_ControlPoint();
        cp->setObject(varControlPoint.getDynamicObject());
        controlPoints.add(cp);
    }

    // check if there is a correct start and end point
    if(controlPoints.size() < 2)
    {
        addControlPoint(0,0);
        addControlPoint(events.getLast()->getTime() * 0.001f, events.getLast()->getPosition());
    }
    else
    {
        ControlPointTimeComparator sorter;
        controlPoints.sort(sorter, true);

        controlPoints.getFirst()->position = 0;
    }

    updateEvents();
    
    playAudioClick = object->getProperty("playAudioClick");
    audioDownbeatPitch = object->getProperty("audioDownbeatPitch");
    audioDownbeatVolume = object->getProperty("audioDownbeatVolume");
    audioBeatPitch = object->getProperty("audioBeatPitch");
    audioBeatVolume = object->getProperty("audioBeatVolume");
    audioCuePitch = object->getProperty("audioCuePitch");
    audioCueVolume = object->getProperty("audioCueVolume");
    audioChannel = object->getProperty("audioChannel");
    
    playMidiClick = object->getProperty("playMidiClick");
    midiDownbeatPitch = object->getProperty("midiDownbeatPitch");
    midiDownbeatVelocity = object->getProperty("midiDownbeatVelocity");
    midiBeatPitch = object->getProperty("midiBeatPitch");
    midiBeatVelocity = object->getProperty("midiBeatVelocity");
    midiCuePitch = object->getProperty("midiCuePitch");
    midiCueVelocity = object->getProperty("midiCueVelocity");
    midiChannel = object->getProperty("midiChannel");
    
    sendOsc = object->getProperty("sendOsc");
    oscDownbeatMessage = object->getProperty("oscDownbeatMessage");
    oscBeatMessage = object->getProperty("oscBeatMessage");
    oscCueMessage = object->getProperty("oscCueMessage");
    oscReceiver = object->getProperty("oscReceiver");
    oscPort = object->getProperty("oscPort");
    
    mute = object->getProperty("mute");
    
    if(object->hasProperty("showName"))
        showName = object->getProperty("showName");
    if(object->hasProperty("staveOffset"))
        staveOffset = object->getProperty("staveOffset");
    if(object->hasProperty("lineOffset"))
        lineOffset = object->getProperty("lineOffset");
    if(object->hasProperty("numberOfLines"))
        numberOfLines = object->getProperty("numberOfLines");
    if(object->hasProperty("numberOfStaves"))
        numberOfStaves = object->getProperty("numberOfStaves");
    if(object->hasProperty("secondaryStaveOffset"))
        secondaryStaveOffset = object->getProperty("secondaryStaveOffset");
}
