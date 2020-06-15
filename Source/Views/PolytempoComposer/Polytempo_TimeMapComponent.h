#pragma once

#include "JuceHeader.h"
#include "Polytempo_CoordinateSystem.h"
#include "Polytempo_Ruler.h"


class Polytempo_TimeMapComponent : public Component,
                                   public ChangeListener
{
public:
    Polytempo_TimeMapComponent();
    ~Polytempo_TimeMapComponent();

    void paint (Graphics&);
    void resized();

    void setRelativeSize(float, Rational);
    void changeListenerCallback (ChangeBroadcaster*);

private:
    float zoomX, zoomY;
    float relativeWidth;
    Rational relativeHeight;
    std::unique_ptr<Polytempo_TimeMapCoordinateSystem> timeMapCoordinateSystem;
    std::unique_ptr<Polytempo_CoordinateSystem>  coordinateSystem;
    Polytempo_TimeRuler         timeRuler;
    Polytempo_PositionRuler     positionRuler;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_TimeMapComponent)
};
