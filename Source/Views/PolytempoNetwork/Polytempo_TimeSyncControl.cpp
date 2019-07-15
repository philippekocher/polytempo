/*
  ==============================================================================

    Polytempo_TimeSyncControl.cpp
    Created: 4 Dec 2017 10:40:17pm
    Author:  christian.schweizer

  ==============================================================================
*/

#include "Polytempo_TimeSyncControl.h"
#include "../../Network/Polytempo_TimeProvider.h"

//==============================================================================
Polytempo_TimeSyncControl::Polytempo_TimeSyncControl()
{
    addAndMakeVisible(syncMasterToggle = new ToggleButton("Master"));
	syncMasterToggle->addListener(this);
    syncMasterToggle->setWantsKeyboardFocus(false);

	addAndMakeVisible(infoField = new Label());
    infoField->setMinimumHorizontalScale(0.1f);
}

Polytempo_TimeSyncControl::~Polytempo_TimeSyncControl()
{
	deleteAllChildren();
}

void Polytempo_TimeSyncControl::paint (Graphics& g)
{
    g.fillAll (Colours::white);   // clear the background
}

void Polytempo_TimeSyncControl::resized()
{
    syncMasterToggle->setBounds(0, 5, getWidth()-20, 18);
	infoField->setBounds(0, 24, getWidth(), 22);
}

void Polytempo_TimeSyncControl::showInfoMessage(String message, Colour color)
{
	stopTimer(TIMER_ID_DURATION);
	stopTimer(TIMER_ID_DELAY);

	newString = message;
	newColor = color;

	startTimer(TIMER_ID_DELAY, DISPLAY_DELAY);
}

void Polytempo_TimeSyncControl::timerCallback(int timerID)
{
	stopTimer(timerID);
	switch(timerID)
	{
	case TIMER_ID_DELAY:
		infoField->setText(newString, dontSendNotification);
		infoField->setColour(infoField->backgroundColourId, newColor);
		if(!newString.isEmpty())
			startTimer(TIMER_ID_DURATION, DISPLAY_DURATION);
		break;
	case TIMER_ID_DURATION:
		resetInfoField();
		break;
	default: 
		DBG("Unknown timerID " + String(timerID));
	}
}

void Polytempo_TimeSyncControl::buttonClicked(Button* button)
{
	if (button == syncMasterToggle)
	{
		Polytempo_TimeProvider::getInstance()->toggleMaster(syncMasterToggle->getToggleState());
		resetInfoField();
	}
}

void Polytempo_TimeSyncControl::resetInfoField()
{
	newString = String();
	newColor = findColour(Label::backgroundColourId);
	startTimer(TIMER_ID_DELAY, DISPLAY_DELAY);
}
