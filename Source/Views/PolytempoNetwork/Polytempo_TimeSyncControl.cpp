/*
  ==============================================================================

    Polytempo_TimeSyncControl.cpp
    Created: 4 Dec 2017 10:40:17pm
    Author:  chris

  ==============================================================================
*/

#include "Polytempo_TimeSyncControl.h"
#include "../../Network/Polytempo_TimeProvider.h"

//==============================================================================
Polytempo_TimeSyncControl::Polytempo_TimeSyncControl()
{
    addAndMakeVisible(syncMasterToggle = new ToggleButton("Sync Master"));
	syncMasterToggle->addListener(this);

	addAndMakeVisible(infoField = new Label());
}

Polytempo_TimeSyncControl::~Polytempo_TimeSyncControl()
{
	deleteAllChildren();
}

void Polytempo_TimeSyncControl::paint (Graphics& g)
{
    g.fillAll (Colours::white);   // clear the background
	g.drawRect(0, 0, getWidth(), getHeight(), 1);
}

void Polytempo_TimeSyncControl::resized()
{
    syncMasterToggle->setBounds(2, 2, getWidth()-4, 30);
	infoField->setBounds(2, getHeight()-32, getWidth()-4, 30);
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
		newString = String::empty;
		newColor = findColour(Label::backgroundColourId);
		startTimer(TIMER_ID_DELAY, DISPLAY_DELAY);
		break;
	default: 
		DBG("Unknown timerID " + String(timerID));
	}
}

void Polytempo_TimeSyncControl::buttonClicked(Button* button)
{
	if (button == syncMasterToggle)
		Polytempo_TimeProvider::getInstance()->initialize(syncMasterToggle->getToggleState(), TIME_SYNC_OSC_PORT);
}
