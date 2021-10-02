#pragma once

#include <JuceHeader.h>

#include "../../../Source/Library/Polytempo_LibCallbackHandler.h"
#define NAME_EVENT_STRING "settings name "

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::Component, ToggleButton::Listener, EventCallbackHandler, TickCallbackHandler, StateCallbackHandler, AsyncUpdater
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void buttonClicked(Button*) override;
    void processEvent(std::string const& message) override;
    void processTick(double tick) override;
    void processState(int state, std::string message) override;
    void handleAsyncUpdate() override;
    
    std::unique_ptr<GroupComponent> groupIn;
    std::unique_ptr<ToggleButton> toggleMaster;

    std::unique_ptr<Label> labelClientName;
    std::unique_ptr<TextEditor> textClientName;
    std::unique_ptr<TextButton> btnSetClientName;

    std::unique_ptr<Label> labelCommand;
    std::unique_ptr<TextEditor> textCommand;
    std::unique_ptr<TextButton> btnSendCommand;

    std::unique_ptr<GroupComponent> groupOut;
    std::unique_ptr<Label> labelNetworkStatus;
    std::unique_ptr<TextEditor> textNetworkStatus;

    std::unique_ptr<ToggleButton> toggleConnected;

    std::unique_ptr<Label> labelTime;
    std::unique_ptr<TextEditor> textTime;

    std::unique_ptr<Label> labelScoreTime;
    std::unique_ptr<TextEditor> textScoreTime;

    std::unique_ptr<Label> labelReceivedCommands;
    std::unique_ptr<TextEditor> textReceivedCommmands;
    
    String timeString;
    OwnedArray<String, CriticalSection> textToAppend;
    String newClientName;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
