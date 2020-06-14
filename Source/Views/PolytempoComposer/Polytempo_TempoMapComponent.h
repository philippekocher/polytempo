#pragma once

#include "JuceHeader.h"
#include "Polytempo_CoordinateSystem.h"
#include "Polytempo_Ruler.h"


class Polytempo_TempoMapComponent : public Component,
                                    public ChangeListener
{
public:
    Polytempo_TempoMapComponent();
    ~Polytempo_TempoMapComponent();

    void paint (Graphics&);
    void resized();
    
    void setRelativeSize(float);
    void changeListenerCallback (ChangeBroadcaster*);

private:
    float zoomX, zoomY;
    float relativeWidth;
    std::unique_ptr<Polytempo_TempoMapCoordinateSystem> tempoMapCoordinateSystem;
    std::unique_ptr<Polytempo_CoordinateSystem>  coordinateSystem;
    Polytempo_TimeRuler         timeRuler;
    Polytempo_TempoRuler        tempoRuler;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_TempoMapComponent)
};
