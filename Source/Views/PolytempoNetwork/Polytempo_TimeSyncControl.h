#pragma once

#include "JuceHeader.h"
#include "../../Network/Polytempo_TimeSyncInfoInterface.h"

#define DISPLAY_DURATION	4000
#define DISPLAY_DELAY		100
#define TIMER_ID_DELAY		1
#define TIMER_ID_DURATION	2

class Polytempo_TimeSyncControl : public Component, Button::Listener, MultiTimer, public Polytempo_TimeSyncInfoInterface
{
public:
    Polytempo_TimeSyncControl();
    ~Polytempo_TimeSyncControl() override;

    void paint(Graphics&) override;
    void resized() override;

    void showInfoMessage(int messageType, String message) override;
    void timerCallback(int timerID) override;

private:
    void buttonClicked(Button* button) override;
    void resetInfoField();

private:
    ToggleButton* syncMasterToggle;
    Label* infoField;
    String newString;
    Colour newColor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_TimeSyncControl)
};
