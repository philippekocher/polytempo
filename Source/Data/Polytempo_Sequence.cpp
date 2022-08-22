#include "Polytempo_Sequence.h"
#include "Polytempo_Composition.h"
#include "../Views/PolytempoComposer/Polytempo_ListComponent.h"
#include "../Preferences/Polytempo_StoredPreferences.h"
#include "../Scheduler/Polytempo_EventDispatcher.h"


Polytempo_Sequence::Polytempo_Sequence(int i) : sequenceID(i)
{
    setName("Sequence "+String(sequenceID));
}

Polytempo_Sequence::~Polytempo_Sequence()
{}


//------------------------------------------------------------------------------
#pragma mark -
#pragma mark accessors

int Polytempo_Sequence::getID()
{
    return sequenceID;
}

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

OwnedArray <Polytempo_Event>& Polytempo_Sequence::getTimedEvents()
{
    Polytempo_EventComparator sorter;
    timedEvents.sort(sorter, true); // true = retain order of equal elements
    return timedEvents;
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

float Polytempo_Sequence::getMaxTime()
{
    return controlPoints.getLast() == nullptr ? 0 : controlPoints.getLast()->time;
}

Rational Polytempo_Sequence::getMaxPosition()
{
    Rational beatPatternMax   = events.getLast() == nullptr ? 0 : events.getLast()->getPosition();
    Rational controlPointsMax = controlPoints.getLast() == nullptr ? 0 : controlPoints.getLast()->position;
    if(beatPatternMax > controlPointsMax) return beatPatternMax;
    else return controlPointsMax;
}

bool Polytempo_Sequence::isVisible()
{
    return visible;
}

bool Polytempo_Sequence::isSoloed()
{
    return solo;
}

bool Polytempo_Sequence::isMuted()
{
    return mute;
}

void Polytempo_Sequence::setName(String s)
{
    if(name != s)
    {
        name = s;
        Polytempo_Composition::getInstance()->setDirty(true);
    }
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

void Polytempo_Sequence::setControlPointTime(int index, float t)
{
    controlPoints[index]->time = t;
}

void Polytempo_Sequence::setControlPointPosition(int index, String& positionString)
{
    if(positionString.toLowerCase() == "end")
    {
        setControlPointPosition(index, events.getLast()->getPosition());
        return;
    }
    
    // parse position string
    StringArray tokens;
    tokens.addTokens(positionString, ":", "");
    tokens.trim();
    
    Rational tempPosition;
    
    if(tokens.size() == 2)
    {
        tempPosition = getPositionForCounter(tokens[0].getIntValue());
        if(tempPosition != 0) setControlPointPosition(index, tempPosition + Rational(tokens[1]));
    }
    else if(tokens.size() == 3)
    {
        tempPosition = getPositionForCounter(tokens[1].getIntValue(), tokens[0]);
        if(tempPosition != 0) setControlPointPosition(index, tempPosition + Rational(tokens[2]));
    }
    else
        setControlPointPosition(index, Rational(tokens[tokens.size() - 1]));
}

void Polytempo_Sequence::setControlPointPosition(int index, Rational position)
{
    controlPoints[index]->position = position;
    controlPoints[index]->positionString = formatPosition(position);
}

String Polytempo_Sequence::formatPosition(Rational position)
{
    if(!Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("simplifiedPosition")) return position.toString();
    if(!events.isEmpty() && position == events.getLast()->getPosition()) return "end";
    
    String beatPatternMarker;
    Rational beatPatternPosition;

    for(Polytempo_BeatPattern* beatPattern : beatPatterns)
    {
        if(beatPattern->getMarker().isNotEmpty()) beatPatternMarker = beatPattern->getMarker();
        beatPatternPosition = beatPattern->getStartPosition();
        
        if(beatPatternPosition <= position &&
           beatPatternPosition + beatPattern->getLength() * beatPattern->getRepeats() > position)
        {
            String result = "";
            int counter = beatPattern->getCurrentCounter();
            Rational length = beatPattern->getLength();
            Rational delta = position - beatPatternPosition;
            while(delta >= length)
            {
                counter++;
                delta = delta - length;
            }
            if(beatPatternMarker.isNotEmpty())
                result += beatPatternMarker+":";
            
            result += String(counter)+":"+delta.toString();
            
            // double check
            if(position != getPositionForCounter(counter, beatPatternMarker) + delta)
                result = position.toString();
            
            return result;
        };
    }
    return position.toString();
}


void Polytempo_Sequence::setControlPointStart(int index, int start)
{
    controlPoints[index]->start = start != 0;
}

void Polytempo_Sequence::setControlPointCue(int index, String cue)
{
    controlPoints[index]->cueIn.setPattern(cue, true);
}

void Polytempo_Sequence::shiftControlPoints(Array<int>* indices, float deltaTime, Rational deltaPosition)
{
    for(int index : *indices)
    {
        controlPoints[index]->time += deltaTime;
        setControlPointPosition(index, controlPoints[index]->position + deltaPosition);
    }
    
    if(!update()) Polytempo_Alert::show("Error", "Invalid operation");
}


void Polytempo_Sequence::setControlPointTempos(int index, float inTempo, float outTempo, float inTempoWeight, float outTempoWeight)
{
    if(inTempo > 0)  controlPoints[index]->tempoIn  = inTempo;
    if(outTempo > 0) controlPoints[index]->tempoOut = outTempo;
    if(inTempoWeight >= 0)   controlPoints[index]->tempoInWeight = inTempoWeight;
    if(outTempoWeight >= 0)  controlPoints[index]->tempoOutWeight = outTempoWeight;

    update();
}

bool Polytempo_Sequence::isTempoConstantAfterPoint(int i)
{
    if(i == controlPoints.size() - 1) return false;
    
    if(fabs(controlPoints[i]->tempoOut - (controlPoints[i+1]->position - controlPoints[i]->position).toFloat() / (controlPoints[i+1]->time - controlPoints[i]->time)) < 1e-6 &&
       fabs(controlPoints[i]->tempoOut - controlPoints[i+1]->tempoIn) < 1e-6)
        return true;
    else
        return false;
}

void Polytempo_Sequence::adjustTime(Array<int>* indices, bool relativeToPreviousPoint)
{
    Polytempo_ControlPoint *c0, *c1;
    float meanTempo;
    
    // a point can't be adjusted if there is no relative point
    if(relativeToPreviousPoint && indices->getFirst() == 0) indices->remove(0);
    if(!relativeToPreviousPoint && indices->getLast() == controlPoints.size()-1) indices->removeLast();

    if(relativeToPreviousPoint)
    {
        for(int index : *indices)
        {
            c0 = controlPoints[index-1];
            c1 = controlPoints[index];
            meanTempo = (c0->tempoOut + c1->tempoIn) * 0.5f;
            c1->time = c0->time + (c1->position - c0->position).toFloat() / meanTempo;
        }
    }
    else
    {
        for(int i=indices->size()-1; i>=0; i--)
        {
            c0 = controlPoints[indices->getUnchecked(i)];
            c1 = controlPoints[indices->getUnchecked(i)+1];
            meanTempo = (c0->tempoOut + c1->tempoIn) * 0.5f;
            c0->time = c1->time - (c1->position - c0->position).toFloat() / meanTempo;
        }
    }
        
    if(!update()) Polytempo_Alert::show("Error", "Invalid operation");
}

void Polytempo_Sequence::adjustPosition(Array<int>* indices, bool relativeToPreviousPoint)
{
    Polytempo_ControlPoint *c0, *c1;
    float meanTempo;

    // a point can't be adjusted if there is no relative point
    if(relativeToPreviousPoint && indices->getFirst() == 0) indices->remove(0);
    if(!relativeToPreviousPoint && indices->getLast() == controlPoints.size()-1) indices->removeLast();

    if(relativeToPreviousPoint)
    {
        for(int index : *indices)
        {
            c0 = controlPoints[index-1];
            c1 = controlPoints[index];
            meanTempo = (c0->tempoOut + c1->tempoIn) * 0.5f;
            setControlPointPosition(index, Rational(c0->position.toFloat() + (c1->time - c0->time) * meanTempo));
        }
    }
    else
    {
        for(int i=indices->size()-1; i>=0; i--)
        {
            c0 = controlPoints[indices->getUnchecked(i)];
            c1 = controlPoints[indices->getUnchecked(i)+1];
            meanTempo = (c0->tempoOut + c1->tempoIn) * 0.5f;
            setControlPointPosition(indices->getUnchecked(i), Rational(c1->position.toFloat() - (c1->time - c0->time) * meanTempo));
        }
    }
    
    if(!update()) Polytempo_Alert::show("Error", "Invalid operation");
}

void Polytempo_Sequence::moveLastControlPointToEnd()
{
    auto lastEvent = events.getLast();
    if(lastEvent == nullptr) return;
    
    int index = controlPoints.size() - 1;
    if(controlPoints.getUnchecked(index)->position != lastEvent->getPosition())
    {
        setControlPointPosition(index, lastEvent->getPosition());
        Array<int> indexArray = Array<int>(index);
        adjustTime(&indexArray, true);
    }
}

void Polytempo_Sequence::adjustTempo(Array<int>* indices)
{
    indices->sort();
    for(int i=1;i<indices->size();i++)
    {
        if(indices->getUnchecked(i) - indices->getUnchecked(i-1) != 1) continue; // only adjust the tempos of two adjacent points
        
        Polytempo_ControlPoint *c0 = controlPoints[indices->getUnchecked(i)-1];
        Polytempo_ControlPoint *c1 = controlPoints[indices->getUnchecked(i)];
        float tempo = (c1->position - c0->position).toFloat() / (c1->time - c0->time);
    
        c0->tempoOut = tempo;
        c1->tempoIn = tempo;
    }

    update();
}

void Polytempo_Sequence::removeControlPoints(Array<int>* indices)
{
    for(int i=indices->size()-1;i>=0;i--)
        controlPoints.remove(indices->getUnchecked(i));

    update();
}

void Polytempo_Sequence::addControlPoint(float t, Rational pos, float tin, float tout)
{
    Polytempo_ControlPoint* point = new Polytempo_ControlPoint();
    
    point->position = pos;
    point->positionString = pos.toString();
    point->time = t;
    
    controlPoints.add(point);
    ControlPointTimeComparator sorter;
    controlPoints.sort(sorter, true);
    
    int index = controlPoints.indexOf(point);
    if(index == 0) point->start = true;
    if(index > 0 && index < controlPoints.size() - 1)
    {
        point->tempoIn  = tin == 0 ? controlPoints[index-1]->tempoOut : tin;
        point->tempoOut = tout == 0 ? controlPoints[index+1]->tempoIn : tout;
    }
    else
    {
        point->tempoIn = tin == 0 ? 0.25f : tin;
        point->tempoOut = tout == 0 ? 0.25f : tout;
    }
    
    update();
    Polytempo_Composition::getInstance()->clearSelectedControlPointIndices();
}

Polytempo_BeatPattern* Polytempo_Sequence::getSelectedBeatPattern()
{
    return beatPatterns[selectedBeatPatternIndex];
}

int Polytempo_Sequence::getSelectedBeatPatternIndex()
{
    return selectedBeatPatternIndex;
}

void Polytempo_Sequence::setSelectedBeatPatternIndex(int index)
{
    selectedBeatPatternIndex = index;
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

    insertBeatPatternAtIndex(selectedBeatPatternIndex, tokens[0], tokens[1].getIntValue(), "+", "");
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
    beatPatterns.remove(selectedBeatPatternIndex);
    
    buildBeatPattern();
    Polytempo_Composition::getInstance()->setDirty(true);
}

void Polytempo_Sequence::buildBeatPattern()
{
    events.clear();
    
    if(beatPatterns.size() == 0)
    {
        Polytempo_Composition::getInstance()->updateContent();
        return;
    }
    
    Rational position = 0;
    int currentCounter = 1;
    
    for(int i=0;i<beatPatterns.size();i++)
    {
        beatPatterns[i]->setCurrentCounter(currentCounter);
        beatPatterns[i]->setStartPosition(position);
        events.addArray(beatPatterns[i]->getEvents(position));
        
        currentCounter = beatPatterns[i]->getCurrentCounter() + beatPatterns[i]->getRepeats();
        position = position + beatPatterns[i]->getLength() * beatPatterns[i]->getRepeats();
    }

    // add a very last event
    Polytempo_Event *event = new Polytempo_Event(eventType_Beat);
    event->setOwned(true);
    event->setPosition(position);
    event->setProperty(eventPropertyString_Pattern, 10);
    event->setValue(events.getLast()->getValue());
    events.add(event);
    
    if(controlPoints.size() == 0 && beatPatterns.size() == 1)
    {
        // add two control points with the very first beat pattern
        addControlPoint(0,0);
        addControlPoint(4,1);
    }

    update();
}

Rational Polytempo_Sequence::getPositionForCounter(int counter, String marker)
{
    String beatPatternMarker;
    int beatPatternCounter;

    for(Polytempo_BeatPattern* beatPattern : beatPatterns)
    {
        if(beatPattern->getMarker().isNotEmpty()) beatPatternMarker = beatPattern->getMarker();
        beatPatternCounter = beatPattern->getCurrentCounter();
        
        if(beatPatternCounter <= counter &&
           beatPatternCounter + beatPattern->getRepeats() > counter)
        {
            if(marker.isEmpty() || marker == beatPatternMarker)
                return (beatPattern->getStartPosition() + beatPattern->getLength() * (counter - beatPatternCounter));
        };
    }
    
    return Rational();
}

std::unique_ptr<Polytempo_ControlPoint> Polytempo_Sequence::getInterpolatedControlPoint(float time)
{
    if(controlPoints.isEmpty()) return nullptr;
    
    std::unique_ptr<Polytempo_ControlPoint> newControlPoint = std::make_unique<Polytempo_ControlPoint>();
    newControlPoint->time = time;
    int i;
    for(i=0; i < controlPoints.size(); i++)
    {
        if (controlPoints.getUnchecked(i)->time > time)
        break;
    }
    if(i == controlPoints.size())
    {
        // extrapolate
        Polytempo_ControlPoint *lastControlPoint = controlPoints.getLast();
        newControlPoint->position = Rational(lastControlPoint->position + (time - lastControlPoint->time) * lastControlPoint->tempoOut);
        newControlPoint->tempoIn = newControlPoint->tempoOut = lastControlPoint->tempoOut;
    }
    else
    {
        // interpolate
        newControlPoint->position = Polytempo_TempoInterpolation::getPosition(time, controlPoints.getUnchecked(i-1), controlPoints.getUnchecked(i));
        newControlPoint->tempoIn = newControlPoint->tempoOut = Polytempo_TempoInterpolation::getTempo(newControlPoint->position, controlPoints.getUnchecked(i-1), controlPoints.getUnchecked(i));
    }
    return newControlPoint;
}

std::unique_ptr<Polytempo_ControlPoint> Polytempo_Sequence::getInterpolatedControlPoint(Rational position)
{
    if(controlPoints.isEmpty()) return nullptr;

    std::unique_ptr<Polytempo_ControlPoint> newControlPoint = std::make_unique<Polytempo_ControlPoint>();
    newControlPoint->position = position;
    int i;
    for(i=0; i < controlPoints.size(); i++)
    {
        if (controlPoints.getUnchecked(i)->position > position)
        break;
    }
    if(i == controlPoints.size())
    {
        // extrapolate
        Polytempo_ControlPoint *lastControlPoint = controlPoints.getLast();
        newControlPoint->time = lastControlPoint->time + (position - lastControlPoint->position).toFloat() / lastControlPoint->tempoOut;
        newControlPoint->tempoIn = newControlPoint->tempoOut = lastControlPoint->tempoOut;
    }
    else
    {
        // interpolate
        newControlPoint->time = Polytempo_TempoInterpolation::getTime(position, controlPoints.getUnchecked(i-1), controlPoints.getUnchecked(i));
        newControlPoint->tempoIn = newControlPoint->tempoOut = Polytempo_TempoInterpolation::getTempo(newControlPoint->position, controlPoints.getUnchecked(i-1), controlPoints.getUnchecked(i));
    }
    return newControlPoint;
}

bool Polytempo_Sequence::update()
{
    //validateControlPoints
    for(int i=0;i<controlPoints.size();i++)
    {
        if((i > 0 && controlPoints[i]->time     <= controlPoints[i-1]->time) ||
           (i > 0 && controlPoints[i]->position <= controlPoints[i-1]->position) ||
           (i < controlPoints.size()-1 && controlPoints[i]->time     >  controlPoints[i+1]->time) ||
           (i < controlPoints.size()-1 && controlPoints[i]->position >= controlPoints[i+1]->position) ||
           controlPoints[i]->position < 0)
        {
            controlPoints.clearQuick(true);
            for(int iBackup=0;iBackup<controlPointsBackup.size();iBackup++)
            {
                controlPoints.add(controlPointsBackup[iBackup]->copy());
            }
            return false;
        }
    }

    // settings for first and last points
    if(controlPoints.size() > 0)
    {
        controlPoints[0]->start = true;
        controlPoints[0]->tempoIn = controlPoints[0]->tempoOut;
        controlPoints[controlPoints.size()-1]->tempoOut = controlPoints[controlPoints.size()-1]->tempoIn;
    }
    
    controlPointsBackup.clearQuick(true);
    for(int i=0;i<controlPoints.size();i++)
    {
        controlPoints[i]->positionString = formatPosition(controlPoints[i]->position);
        controlPointsBackup.add(controlPoints[i]->copy());
    }
    
    // set the time for all events
    int cpIndex = -1;
    
    for(int i=0;i<events.size();i++)
    {
        Polytempo_Event *event = events[i];
        
       if(controlPoints[cpIndex+1] != nullptr &&
          event->getPosition() >= controlPoints[cpIndex+1]->position &&
          controlPoints.size() > cpIndex + 2)
          cpIndex++;
                
        Polytempo_ControlPoint *cp1 = controlPoints[cpIndex];
        Polytempo_ControlPoint *cp2 = controlPoints[cpIndex+1];
        
        if(cp1 == nullptr || cp2 == nullptr)
            event->setProperty(eventPropertyString_Time, NAN);
        else if(cp2->start)
        {
            float temp = Polytempo_TempoInterpolation::getTime(event->getPosition(), cp1, cp2);
            if(temp == cp1->time)
                event->setProperty(eventPropertyString_Time, temp);
            else
                event->setProperty(eventPropertyString_Time, NAN);
        }
        else if(Polytempo_TempoInterpolation::validateCurve(cp1,cp2))
            event->setProperty(eventPropertyString_Time, Polytempo_TempoInterpolation::getTime(event->getPosition(), cp1, cp2));
        else
            event->setProperty(eventPropertyString_Time, NAN);
        
        
        if(event->getType() == eventType_Beat)
        {
            if(cp2 != nullptr && !cp2->cueIn.getPattern().isEmpty() &&
               cp2->position - cp2->cueIn.getLength() <= event->getPosition())
                event->setProperty(eventPropertyString_Cue, 1);
            else
                event->removeProperty(eventPropertyString_Cue);
        }
    }
    
    // set the beat durations and store them in the array timedEvents
    timedEvents.clearQuick(true);

    float eventTime = NAN;
    for(int i=events.size()-1;i>=0;i--)
    {
        Polytempo_Event *event = new Polytempo_Event(*events[i]);

        if(event->getType() == eventType_Beat)
        {
            if(event->hasDefinedTime())
            {
                if(eventTime != eventTime) // time is NaN
                {
                    event->setProperty(eventPropertyString_Duration, 1.0f);
                    event->setProperty(eventPropertyString_Pattern, int(float(event->getProperty(eventPropertyString_Pattern)) * 0.1f) * 10);
                }
                else
                {
                    event->setProperty(eventPropertyString_Duration, eventTime - float(event->getProperty(eventPropertyString_Time)));
                }
            }
            eventTime = float(event->getProperty(eventPropertyString_Time));
        }
        if(event->hasDefinedTime())
        {
            event->setProperty("~sequence", sequenceID);
            timedEvents.add(event);
        }
    }
    
    // add cue-in events for start control points
    for(Polytempo_ControlPoint* cp : controlPoints)
    {
        if(cp->start && !cp->cueIn.getPattern().isEmpty())
        {
            float cueInStartTime = cp->time - cp->cueIn.getLength().toFloat() / cp->tempoOut;
            float time, nextTime = cp->time;
            Array<Polytempo_Event*> tempEvents = cp->cueIn.getEvents(0);
            
            for(int i=tempEvents.size()-1;i>=0;i--)
            {
                Polytempo_Event* cueInEvent = tempEvents[i];
                if(cueInEvent->getType() == eventType_Beat)
                {
                    cueInEvent->setProperty("~sequence", sequenceID);
                    time = cueInStartTime + cueInEvent->getPosition().toFloat() / cp->tempoOut;
                    cueInEvent->setProperty(eventPropertyString_Time, time);
                    cueInEvent->setProperty(eventPropertyString_Cue, 1);
                    cueInEvent->setProperty(eventPropertyString_Duration, nextTime - time);
                    nextTime = time;
                }
                timedEvents.add(cueInEvent);
            }
        }
    }
    
    Polytempo_Composition::getInstance()->updateContent();
    Polytempo_Composition::getInstance()->setDirty(true);
    
    return true;
}

void Polytempo_Sequence::addPlaybackPropertiesToEvent(Polytempo_Event* event)
{
    int pattern = event->getProperty(eventPropertyString_Pattern);
        
    if(playAudioClick)
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
    if(playMidiClick)
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
                event->setProperty("midiVelocity", midiBeatVelocity);
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
    
        if(event->getType() == eventType_Beat)
        {
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
        }
        else if(event->getType() == eventType_Marker)
        {
            oscString = "/marker value " + event->getValue().toString();
        }
        
        
        StringArray tokens;
        tokens.addTokens(oscString, " ", "");
        tokens.removeEmptyStrings();
        tokens.trim();
        
        oscEvent->setProperty("address", tokens[0]);
        oscEvent->setProperty("senderID", "sequence"+String(sequenceID));
        
        var message;
        for(int i=1;i<tokens.size();i++) message.append(tokens[i]);
        
        oscEvent->setProperty("message", message);
        
        oscEvent->setSyncTime(event->getSyncTime());
        
        return oscEvent;
    }
    
    return nullptr;
}

void Polytempo_Sequence::updateOscReceiver()
{
    Polytempo_Event *event = new Polytempo_Event(eventType_AddSender);
    event->setProperty("senderID", "sequence"+String(sequenceID));
    event->setProperty("ip", oscReceiver);
    event->setProperty("port", oscPort);
    event->setProperty("tick", sendOsc ? 1 : 0);

    Polytempo_EventDispatcher::getInstance()->broadcastEvent(event);
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
    object->setProperty("solo", solo);
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
        cp->positionString = formatPosition(cp->position);
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
    }

    update();
    
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
    
    updateOscReceiver();
    
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
