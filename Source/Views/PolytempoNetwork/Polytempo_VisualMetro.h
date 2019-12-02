#ifndef __Polytempo_VisualMetro__
#define __Polytempo_VisualMetro__

#include "../../Scheduler/Polytempo_EventObserver.h"

class Polytempo_VisualConductorComponent;

class Polytempo_VisualMetro : public  Component,
                              private Timer,
                              public  Polytempo_EventObserver
{
public:
    Polytempo_VisualMetro();
    ~Polytempo_VisualMetro();

    void paint (Graphics& g);
    
    void setForegroundColour (Colour& colour);
    void setBackgroundColour (Colour& colour);
    void setFrameColour (Colour& colour);
    void setWidth (float val);
    
    void timerCallback();
    void eventNotification(Polytempo_Event *event);

private:
    float width;
    float pos, subpos;
    float increment;
    int timeInterval;
    float x,y;
    float tempoFactor;
    float exponentMain, exponentSub;
    float beatDuration;
    int beatSubdivision, pattern;
    Colour foregroundColour;
    Colour normalColour, cueColour;
    bool shouldStop;
    bool linear;
    int holdMax;

    Polytempo_VisualConductorComponent *hComponent;
    Polytempo_VisualConductorComponent *vComponent;
};

#endif   // __Polytempo_VisualMetro__
