#pragma once

#include "../../Scheduler/Polytempo_EventObserver.h"

class Polytempo_VisualConductorComponent;

class Polytempo_VisualMetro : public Component,
                              Timer,
                              public Polytempo_EventObserver
{
public:
    Polytempo_VisualMetro();
    ~Polytempo_VisualMetro() override;

    void paint(Graphics& g) override;

    void setForegroundColour(Colour& colour);
    void setBackgroundColour(Colour& colour);
    void setFrameColour(Colour& colour);
    void setWidth(float val);

    void timerCallback() override;
    void eventNotification(Polytempo_Event* event) override;

private:
    float width;
    float pos, subpos;
    float increment;
    int timeInterval;
    float x, y;
    float tempoFactor;
    float exponentMain, exponentSub;
    float beatDuration;
    int beatSubdivision, pattern;
    Colour foregroundColour;
    Colour normalColour, cueColour;
    bool shouldStop;
    bool linear;
    int holdMax;

    Polytempo_VisualConductorComponent* hComponent;
    Polytempo_VisualConductorComponent* vComponent;
};
