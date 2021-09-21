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

void Polytempo_TimeSyncControl::paint(Graphics& g)
{
    g.fillAll(Colours::white); // clear the background
}

void Polytempo_TimeSyncControl::resized()
{
    syncMasterToggle->setBounds(0, 5, getWidth() - 20, 18);
    infoField->setBounds(0, 24, getWidth(), 22);
}

void Polytempo_TimeSyncControl::showInfoMessage(int messageType, String message)
{
    stopTimer(TIMER_ID_DURATION);
    stopTimer(TIMER_ID_DELAY);

    Colour c;
    switch (messageType)
    {
    case Polytempo_TimeProvider::MessageType_Info: c = Colours::lightgreen;
        break;
    case Polytempo_TimeProvider::MessageType_Warning: c = Colours::yellow;
        break;
    case Polytempo_TimeProvider::MessageType_Error: c = Colours::orangered;
        break;
    default: c = Colours::grey;
    }

    newString = message;
    newColor = c;

    startTimer(TIMER_ID_DELAY, DISPLAY_DELAY);
}

void Polytempo_TimeSyncControl::timerCallback(int timerID)
{
    stopTimer(timerID);
    switch (timerID)
    {
    case TIMER_ID_DELAY:
        infoField->setText(newString, dontSendNotification);
        infoField->setColour(infoField->backgroundColourId, newColor);
        if (!newString.isEmpty())
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
