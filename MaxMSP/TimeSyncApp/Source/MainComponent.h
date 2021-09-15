#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::Component, ToggleButton::Listener
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
    std::unique_ptr<ToggleButton> toggle;
    std::unique_ptr<TextEditor> textCommand;
    std::unique_ptr<TextButton> sendCommand;
    std::unique_ptr<TextEditor> textLog;

    static void eventCallback(std::string eventAsString);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
