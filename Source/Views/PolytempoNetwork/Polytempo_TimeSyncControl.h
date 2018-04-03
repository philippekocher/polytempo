/*
  ==============================================================================

    Polytempo_TimeSyncControl.h
    Created: 4 Dec 2017 10:40:17pm
    Author:  chris

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#define DISPLAY_DURATION	4000
#define DISPLAY_DELAY		100
#define TIMER_ID_DELAY		1
#define TIMER_ID_DURATION	2

//==============================================================================
/*
*/
class Polytempo_TimeSyncControl    : public Component, Button::Listener, MultiTimer
{
public:
    Polytempo_TimeSyncControl();
    ~Polytempo_TimeSyncControl();

    void paint (Graphics&) override;
    void resized() override;

	void showInfoMessage(String message, Colour color);
	void timerCallback(int timerID) override;

private:
	void buttonClicked(Button* button) override;
	void resetInfoField();

private:
	ToggleButton	*syncMasterToggle;
	Label			*infoField;
	String			newString;
	Colour			newColor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_TimeSyncControl)
};
