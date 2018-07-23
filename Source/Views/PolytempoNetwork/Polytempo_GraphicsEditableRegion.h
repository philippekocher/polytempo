/*
  ==============================================================================

    Polytempo_GraphicsEditableRegion.h
    Created: 27 Jul 2017 8:40:18am
    Author:  chris

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "Polytempo_GraphicsAnnotation.h"
#include "Polytempo_GraphicsPalette.h"

#define MIN_MOUSE_DOWN_TIME_MS 500
#define MIN_INTERVAL_BETWEEN_REPAINTS_MS 20
#define FREE_HAND_LINE_THICKNESS 2
#define DISTANCE_REFERENCEPOINT_TO_BUTTONS 50
#define TIMER_ID_REPAINT 1000
#define TIMER_ID_AUTO_ACCEPT 1001
#define AUTO_ACCEPT_INTERVAL_MS	5000

class FontSizeCallback;

//==============================================================================
/*
*/
class Polytempo_GraphicsEditableRegion : public Component
{
public:
	Polytempo_GraphicsEditableRegion();
    ~Polytempo_GraphicsEditableRegion();


    
protected:
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_GraphicsEditableRegion)
};
