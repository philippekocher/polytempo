#include "MainComponent.h"

#include "../../../Source/Library/Polytempo_LibMain.h"

//==============================================================================
MainComponent::MainComponent()
{
    toggle.reset(new ToggleButton("Master"));
    toggle->addListener(this);
    addAndMakeVisible(toggle.get());

    textCommand.reset(new TextEditor());
    addAndMakeVisible(textCommand.get());

    sendCommand.reset(new TextButton("Send Command"));
    sendCommand->addListener(this);
    addAndMakeVisible(sendCommand.get());

    textLog.reset(new TextEditor());
    addAndMakeVisible(textLog.get());

    setSize (600, 400);

    Polytempo_LibMain::current()->initialize(47522, false);
}

MainComponent::~MainComponent()
{
    Polytempo_LibMain::release();
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    toggle->setBounds(0, 0, 200, 30);
    textCommand->setBounds(0, 40, 200, 30);
    sendCommand->setBounds(210, 40, 150, 30);
    textLog->setBounds(0, 80, getWidth(), getHeight() - 80);
}

void MainComponent::buttonClicked(Button* btn)
{
    if (btn == toggle.get())
    {
        Polytempo_LibMain::current()->toggleMaster(toggle->getToggleState());
    }
    else if(btn == sendCommand.get())
    {
        Polytempo_LibMain::current()->sendEvent(textCommand->getText().toStdString(), "");
    }
}
