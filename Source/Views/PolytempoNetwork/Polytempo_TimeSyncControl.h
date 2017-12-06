/*
  ==============================================================================

    Polytempo_TimeSyncControl.h
    Created: 4 Dec 2017 10:40:17pm
    Author:  chris

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#define DISPLAY_DURATION	3000

//==============================================================================
/*
*/
class Polytempo_TimeSyncControl    : public Component, ButtonListener, Timer
{
public:
    Polytempo_TimeSyncControl();
    ~Polytempo_TimeSyncControl();

    void paint (Graphics&) override;
    void resized() override;

	void showInfoMessage(String message, Colour color);
	void timerCallback() override;

private:
	void buttonClicked(Button* button) override;

private:
	ToggleButton	*syncMasterToggle;
	Label			*infoField;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_TimeSyncControl)
};
