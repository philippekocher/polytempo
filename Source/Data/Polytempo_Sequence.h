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

#ifndef __Polytempo_Sequence__
#define __Polytempo_Sequence__

#include "Polytempo_TempoInterpolation.h"
#include "Polytempo_BeatPattern.h"
#include "Polytempo_Score.h"

class Polytempo_ListComponent;

class Polytempo_Sequence
{
public:
    Polytempo_Sequence();
    ~Polytempo_Sequence();
        
    String getName();
    Colour getColour();
    
    OwnedArray <Polytempo_ControlPoint>* getControlPoints();
    Polytempo_ControlPoint* getControlPoint(int);
    
    OwnedArray <Polytempo_BeatPattern>* getBeatPatterns();
    Polytempo_BeatPattern* getBeatPattern(int);
    
    OwnedArray <Polytempo_Event>& getEvents();
    Polytempo_Event* getEvent(int);
    
    bool isVisible();
    
    void setIndex(int);
    void setName(String);
    void setColour(Colour);
    void setVisibility(bool);
    void setBeatPatternListComponent(Polytempo_ListComponent*);
    
    bool validateNewControlPointPosition(float t, Rational pos);
    bool validateControlPoint(int index, float t, Rational pos);
    void setControlPointValues(int index, float t, Rational pos, float inTempo, float outTempo, float inTempoWeight, float outTempoWeight);
    void setControlPointPosition(int index, float t, Rational pos);
    void setControlPointTempo(int index, float inTempo, float outTempo);
    bool allowAdjustTime(int index);
    void adjustTime(int index);
    void adjustPosition(int index);
    void adjustTempo(int index);
    void removeControlPoint(int index);
    
    bool isTempoConstantAfterPoint(int);
    
    void addControlPoint(float t, Rational pos, float tin=0, float tout=0);
    
    int getSelectedBeatPattern();
    void setSelectedBeatPattern(int index);
    void addBeatPattern();
    void insertBeatPattern();
    void insertBeatPatternAtIndex(int index, const String& pattern, int repeats=1, const String& counter=String(), const String& marker=String());
    void removeSelectedBeatPattern();
    
    void buildBeatPattern();
    void updateEvents();
    
    void addPlaybackPropertiesToEvent(Polytempo_Event*);
    Polytempo_Event* getOscEvent(Polytempo_Event*);
    
    // serialisation
    DynamicObject* getObject();
    void setObject(DynamicObject* object);

private:    
    String name;
    int sequenceIndex = 0;
    Colour colour = Colours::white;
    bool visible = true;

    OwnedArray <Polytempo_ControlPoint> controlPoints;
    OwnedArray <Polytempo_BeatPattern> beatPatterns;
    int selectedBeatPattern = -1;
    OwnedArray <Polytempo_Event> events;
    
    Polytempo_ListComponent* beatPatternListComponent = nullptr;
    
    friend class Polytempo_SequencePlaybackSettings;
    bool  playAudioClick = true;
    int   audioDownbeatPitch = 100;
    float audioDownbeatVolume = 1.0;
    int   audioBeatPitch = 95;
    float audioBeatVolume = 1.0;
    int   audioCuePitch = 90;
    float audioCueVolume = 1.0;
    int   audioChannel = 1;
    
    bool  playMidiClick = false;
    int   midiDownbeatPitch = 100;
    int   midiDownbeatVelocity = 127;
    int   midiBeatPitch = 95;
    int   midiBeatVelocity = 127;
    int   midiCuePitch = 90;
    int   midiCueVelocity = 127;
    int   midiChannel = 1;
  
    bool   sendOsc = false;
    String oscDownbeatMessage = "/beat pattern #pattern duration #duration";
    String oscBeatMessage     = "/beat pattern #pattern duration #duration";
    String oscCueMessage      = "/beat pattern #pattern duration #duration cue 1";

    String oscReceiver        = "#broadcast";
    String oscPort            = "47522";
    
    friend class Polytempo_SequenceControlComponent;
    bool mute = false;
};


#endif  // __Polytempo_Sequence__
