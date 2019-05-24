/*
  ==============================================================================

    Polytempo_TimeSyncControl.cpp
    Created: 4 Dec 2017 10:40:17pm
    Author:  christian.schweizer

  ==============================================================================
*/

#include "Polytempo_TimeSyncControl.h"
#include "../../Network/Polytempo_TimeProvider.h"
#include "../../Network/Polytempo_NetworkSupervisor.h"
#include "../../Misc/Polytempo_Alerts.h"
#include "../../Network/Polytempo_NetworkInterfaceManager.h"

//==============================================================================
Polytempo_TimeSyncControl::Polytempo_TimeSyncControl()
{
    addAndMakeVisible(syncMasterToggle = new ToggleButton("Sync Master"));
	syncMasterToggle->addListener(this);
    syncMasterToggle->setWantsKeyboardFocus(false);

	addAndMakeVisible(optionsButton = new Polytempo_Button("...", Polytempo_Button::buttonType_black));
	optionsButton->addListener(this);
	optionsButton->setWantsKeyboardFocus(false);

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
	optionsButton->setBounds(getWidth() - 20, 5, 20, 18);
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

void Polytempo_TimeSyncControl::handlePopupMenuCallback(int returnValue)
{
	if (returnValue == 1)
	{
		// unicast flood
		if (Polytempo_NetworkInterfaceManager::getInstance()->getSelectedIpAddress().subnetMask.address[2] < 255)
		{
			auto alertLambda = ([](int result) {
				if (result) {
					MouseCursor::showWaitCursor();
					Polytempo_NetworkSupervisor::getInstance()->unicastFlood();
					MouseCursor::hideWaitCursor();
				}
			});
			Polytempo_OkCancelAlert::show("Unicast Flood", "Flooding this network may take several minutes. Proceed?", ModalCallbackFunction::create(alertLambda));
		}
		else
		{
			MouseCursor::showWaitCursor();
			Polytempo_NetworkSupervisor::getInstance()->unicastFlood();
			MouseCursor::hideWaitCursor();
		}
	}
	else if (returnValue == 2)
	{
		// manual connect
		AlertWindow* alertWindow = new AlertWindow("Manual Connect", "Connect to Master-IP-Address:", AlertWindow::NoIcon, this);
		
		auto alertLambda = ([alertWindow](int result) {
			if (result == 1) {
				String ip = alertWindow->getTextEditorContents("ip");
				Polytempo_NetworkSupervisor::getInstance()->manualConnect(ip);
			}
		});
		
		alertWindow->addTextEditor("ip", Polytempo_NetworkInterfaceManager::getInstance()->getSelectedIpAddress().ipAddress.toString().upToLastOccurrenceOf(".", true, true));
		alertWindow->addButton("OK", 1, KeyPress(KeyPress::returnKey, 0, 0));
		alertWindow->addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));
		alertWindow->enterModalState(false, ModalCallbackFunction::create(alertLambda), true);
	}
}

void Polytempo_TimeSyncControl::buttonClicked(Button* button)
{
	if (button == syncMasterToggle)
	{
		Polytempo_TimeProvider::getInstance()->toggleMaster(syncMasterToggle->getToggleState());
		resetInfoField();
	}
	if (button == optionsButton)
	{
		PopupMenu m;
		m.addItem(1, "Unicast Flood");
		m.addItem(2, "Manual connect", !Polytempo_TimeProvider::getInstance()->isMaster());

		m.showMenuAsync(PopupMenu::Options().withTargetComponent(optionsButton), new PopupMenuCallback(this));
	}
}

void Polytempo_TimeSyncControl::resetInfoField()
{
	newString = String();
	newColor = findColour(Label::backgroundColourId);
	startTimer(TIMER_ID_DELAY, DISPLAY_DELAY);
}
