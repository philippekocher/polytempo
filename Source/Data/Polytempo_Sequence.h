#pragma once

#include "Polytempo_TempoInterpolation.h"
#include "Polytempo_BeatPattern.h"


class Polytempo_ListComponent;

class Polytempo_Sequence
{
public:
    Polytempo_Sequence(int);
    ~Polytempo_Sequence();
        
    int getID();
    String getName();
    Colour getColour();
    
    OwnedArray <Polytempo_ControlPoint>* getControlPoints();
    Polytempo_ControlPoint* getControlPoint(int);
    
    OwnedArray <Polytempo_BeatPattern>* getBeatPatterns();
    Polytempo_BeatPattern* getBeatPattern(int);
    
    OwnedArray <Polytempo_Event>& getTimedEvents();
    OwnedArray <Polytempo_Event>& getEvents();
    Polytempo_Event* getEvent(int);
    
    float getMinTime();
    float getMaxTime();
    Rational getMaxPosition();
    
    bool isVisible();
    
    bool isSoloed();
    bool isMuted();
    
    void setName(String);
    void setColour(Colour);
    void setVisibility(bool);
    void setBeatPatternListComponent(Polytempo_ListComponent*);
    
    bool validateNewControlPointPosition(float t, Rational pos);
    void setControlPointTime(int index, float t);
    void setControlPointPosition(int index, String& positionString);
    void setControlPointPosition(int index, Rational position);
    void setControlPointStart(int index, int start);
    void setControlPointCue(int index, String cue);
    void shiftControlPoints(Array<int>* indices, float deltaTime, Rational deltaPosition);
    void setControlPointTempos(int index, float inTempo, float outTempo, float inTempoWeight = -1, float outTempoWeight = -1);
    void adjustTime(Array<int>* indices, bool relativeToPreviousPoint = true);
    void adjustPosition(Array<int>* indices, bool relativeToPreviousPoint = true);
    void moveLastControlPointToEnd();
    void adjustTempo(Array<int>* indices);
    void removeControlPoints(Array<int>* indices);
    
    bool isTempoConstantAfterPoint(int);
    
    void addControlPoint(float t, Rational pos, float tin=0, float tout=0);
    
    Polytempo_BeatPattern* getSelectedBeatPattern();
    int getSelectedBeatPatternIndex();
    void setSelectedBeatPatternIndex(int index);
    void addBeatPattern();
    void insertBeatPattern();
    void insertBeatPatternAtIndex(int index, const String& pattern, int repeats=1, const String& counter=String(), const String& marker=String());
    void removeSelectedBeatPattern();
    
    void buildBeatPattern();
    Rational getPositionForCounter(int counter, String marker = String());
    std::unique_ptr<Polytempo_ControlPoint> getInterpolatedControlPoint(float time);
    std::unique_ptr<Polytempo_ControlPoint> getInterpolatedControlPoint(Rational position);
    bool update();
    
    void addPlaybackPropertiesToEvent(Polytempo_Event*);
    Polytempo_Event* getOscEvent(Polytempo_Event*);
    void updateOscReceiver();
    
    // serialisation
    DynamicObject* getObject();
    void setObject(DynamicObject* object);

private:    
    String formatPosition(Rational position);

    String name;
    int sequenceID;
    Colour colour = Colours::white;
    bool visible = true;

    OwnedArray <Polytempo_ControlPoint> controlPoints;
    OwnedArray <Polytempo_ControlPoint> controlPointsBackup;
    OwnedArray <Polytempo_BeatPattern> beatPatterns;
    int selectedBeatPatternIndex = -1;
    OwnedArray <Polytempo_Event> events;        // events given by the beat patterns
    OwnedArray <Polytempo_Event> timedEvents;   // timed events used for playback and export

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

    String oscReceiver        = "127.0.0.1";
    String oscPort            = "47522";
    
    friend class Polytempo_SequenceControlComponent;
    bool solo = false;
    bool mute = false;

    friend class Polytempo_GraphicExportView;
    friend class Polytempo_SequenceGraphicalSettings;
    bool showName = true;
    int staveOffset = 100;
    int numberOfStaves = 1; // 2 for piano staff etc.
    int secondaryStaveOffset = 60;
    int lineOffset = 8;
    int numberOfLines = 5;
};
