#include "MainComponent.h"
#include "../../../Source/Library/Polytempo_LibApi.h"
#include "../../../Source/Network/Polytempo_PortDefinition.h"
#define NAME_EVENT_STRING "settings name "

//==============================================================================
MainComponent::MainComponent()
{
    toggleOn.reset(new ToggleButton("On"));
    toggleOn->addListener(this);
    toggleOn->setToggleState(true, dontSendNotification);
    addAndMakeVisible(toggleOn.get());
    
    // In
    groupIn.reset(new GroupComponent("In", "In"));
    addAndMakeVisible(groupIn.get());

    toggleMaster.reset(new ToggleButton("Master"));
    toggleMaster->addListener(this);
    addAndMakeVisible(toggleMaster.get());

    labelClientName.reset(new Label("ClientName", "Client Name"));
    addAndMakeVisible(labelClientName.get());

    textClientName.reset(new TextEditor());
    textClientName->setText("DemoApp");
    addAndMakeVisible(textClientName.get());

    btnSetClientName.reset(new TextButton("Set"));
    btnSetClientName->addListener(this);
    addAndMakeVisible(btnSetClientName.get());

    labelCommand.reset(new Label("Command", "Command"));
    addAndMakeVisible(labelCommand.get());

    textCommand.reset(new TextEditor());
    addAndMakeVisible(textCommand.get());

    btnSendCommand.reset(new TextButton("Send"));
    btnSendCommand->addListener(this);
    addAndMakeVisible(btnSendCommand.get());

    // Out
    groupOut.reset(new GroupComponent("Out", "Out"));
    addAndMakeVisible(groupOut.get());

    toggleConnected.reset(new ToggleButton("Connected"));
    toggleConnected->setEnabled(false);
    addAndMakeVisible(toggleConnected.get());

    labelNetworkStatus.reset(new Label("Network Status", "Network Status"));
    addAndMakeVisible(labelNetworkStatus.get());

    textNetworkStatus.reset(new TextEditor());
    textNetworkStatus->setReadOnly(true);
    addAndMakeVisible(textNetworkStatus.get());

    labelTime.reset(new Label("SyncTime", "Sync Time"));
    labelTime->setEnabled(false);
    addAndMakeVisible(labelTime.get());

    textTime.reset(new TextEditor());
    textTime->setReadOnly(true);
    textTime->setEnabled(false);
    addAndMakeVisible(textTime.get());

    labelScoreTime.reset(new Label("ScoreTime", "Score Time"));
    addAndMakeVisible(labelScoreTime.get());

    textScoreTime.reset(new TextEditor());
    textScoreTime->setReadOnly(true);
    addAndMakeVisible(textScoreTime.get());

    labelReceivedCommands.reset(new Label("ReceivedCommands", "Received Commands"));
    addAndMakeVisible(labelReceivedCommands.get());

    textReceivedCommmands.reset(new TextEditor());
    textReceivedCommmands->setMultiLine(true);
    textReceivedCommmands->setReadOnly(true);
    addAndMakeVisible(textReceivedCommmands.get());

    setSize (600, 400);

    setOnOff(true);
}

MainComponent::~MainComponent()
{
    setOnOff(false);
}

void MainComponent::setOnOff(bool on)
{
    if(on)
    {
        PolytempoEventCallbackOptions eventOptions;
        eventOptions.ignoreTimeTag = true;
        PolytempoTickCallbackOptions tickOptions;
        PolytempoStateCallbackOptions stateOptions;
        stateOptions.callOnChangeOnly = true;
        
        const auto applicationName = JUCEApplication::getInstance()->getApplicationName().toStdString();
        
        // This Example uses the POLYTEMPO SERVER port, means it can not be used in connection with the real polytempo server, since every advertise port can only be used once
        polytempo_initialize(POLYTEMPO_NETWORK_PORT_SERVER, toggleMaster->getToggleState(), applicationName);
        polytempo_registerSimpleEventCallback(this, eventOptions);
        polytempo_registerTickCallback(this, tickOptions);
        polytempo_registerStateCallback(this, stateOptions);
        polytempo_sendSimpleEvent(NAME_EVENT_STRING + textClientName->getText().toStdString());
    }
    else
    {
        polytempo_unregisterSimpleEventCallback(this);
        polytempo_unregisterTickCallback(this);
        polytempo_unregisterStateCallback(this);
        polytempo_release();
    }
}
//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    const int border = 8;
    const int rowHeight = 30;
    const int labelWidth = 100;
    const int buttonWidth = 80;

    toggleOn->setBounds(0, 0, getWidth(), rowHeight);
    
    groupIn->setBounds(0, rowHeight, getWidth(), 3 * rowHeight + 4 * border);
    toggleMaster->setBounds(groupIn->getX() + border, groupIn->getY() +  2 * border, groupIn->getWidth() - 2 * border, rowHeight);

    labelClientName->setBounds(groupIn->getX() + border, groupIn->getY() + 2 * border + rowHeight, labelWidth, rowHeight);
    textClientName->setBounds(groupIn->getX() + labelWidth + 2 * border, groupIn->getY() + 2 * border + rowHeight, groupIn->getWidth() - labelWidth - 4 * border - buttonWidth, rowHeight);
    btnSetClientName->setBounds(groupIn->getRight() - border - buttonWidth, groupIn->getY() + 2 * border + rowHeight, buttonWidth, rowHeight);

    labelCommand->setBounds(groupIn->getX() + border, groupIn->getY() + 3 * border + 2 * rowHeight, labelWidth, rowHeight);
    textCommand->setBounds(groupIn->getX() + labelWidth + 2 * border, groupIn->getY() + 3 * border + 2 * rowHeight, groupIn->getWidth() - labelWidth - 4 * border - buttonWidth, rowHeight);
    btnSendCommand->setBounds(groupIn->getRight() - border - buttonWidth, groupIn->getY() + 3 * border + 2 * rowHeight, buttonWidth, rowHeight);

    groupOut->setBounds(0, groupIn->getBottom(), getWidth(), getHeight() - groupIn->getBottom());
    toggleConnected->setBounds(groupOut->getX() + border, groupOut->getY() + 2 * border, groupOut->getWidth() - 2 * border, rowHeight);

    labelNetworkStatus->setBounds(groupOut->getX() + border, groupOut->getY() + 2 * border + rowHeight, labelWidth, rowHeight);
    textNetworkStatus->setBounds(groupOut->getX() + labelWidth + 2 * border, groupOut->getY() + 2 * border + rowHeight, groupOut->getWidth() - labelWidth - 3 * border, rowHeight);

    labelTime->setBounds(groupOut->getX() + border, groupOut->getY() + 3 * border + 2 * rowHeight, labelWidth, rowHeight);
    textTime->setBounds(groupOut->getX() + labelWidth + 2 * border, groupOut->getY() + 3 * border + 2 * rowHeight, groupOut->getWidth() - labelWidth - 3 * border, rowHeight);

    labelScoreTime->setBounds(groupOut->getX() + border, groupOut->getY() + 4 * border + 3 * rowHeight, labelWidth, rowHeight);
    textScoreTime->setBounds(groupOut->getX() + labelWidth + 2 * border, groupOut->getY() + 4 * border + 3 * rowHeight, groupOut->getWidth() - labelWidth - 3 * border, rowHeight);

    labelReceivedCommands->setBounds(groupOut->getX() + border, groupOut->getY() + 5 * border + 4 * rowHeight, groupOut->getWidth() - 2 * border, rowHeight);
    textReceivedCommmands->setBounds(groupOut->getX() + border, groupOut->getY() + 5 * border + 5 * rowHeight, groupOut->getWidth() - 2 * border, groupOut->getHeight() - 6 * border - 5 * rowHeight);
}

void MainComponent::buttonClicked(Button* btn)
{
    if(btn == toggleOn.get())
    {
        setOnOff(btn->getToggleState());
    }
    else if (btn == toggleMaster.get())
    {
        polytempo_toggleMaster(toggleMaster->getToggleState());
    }
    else if(btn == btnSetClientName.get())
    {
        polytempo_sendSimpleEvent(NAME_EVENT_STRING + textClientName->getText().toStdString());
    }
    else if(btn == btnSendCommand.get())
    {
        polytempo_sendSimpleEvent(textCommand->getText().toStdString());
    }
}

void MainComponent::processEvent(std::string const& message)
{
    
    String msg(message);
    if(msg.startsWith(NAME_EVENT_STRING))
    {
        newClientName = msg.substring(String(NAME_EVENT_STRING).length()).upToFirstOccurrenceOf(" ", false, true);
    }
    else
    {
        textToAppend.add(new String(msg + NewLine::getDefault()));
    }

    triggerAsyncUpdate();
}

void MainComponent::processTick(double tick)
{
    timeString = String(tick, 3);
    triggerAsyncUpdate();
}

void MainComponent::processState(int state, std::string message, std::vector<std::string> peers)
{
    // TODO: defines
    Colour c;
    switch (state)
    {
    case 0: c = Colours::darkgreen;
        break;
    case 1: c = Colours::orange;
        break;
    case 2: c = Colours::darkred;
        break;
    default: c = Colours::grey;
    }

    const MessageManagerLock mmLock;

    textNetworkStatus->setText(message);
    textNetworkStatus->setColour(TextEditor::backgroundColourId, c);

    toggleConnected->setToggleState(state == 0, dontSendNotification);
}

void MainComponent::processMasterChanged(bool isMaster)
{
    toggleMaster->setToggleState(isMaster, dontSendNotification);
}

void MainComponent::handleAsyncUpdate()
{
    if(textScoreTime->getText() != timeString)
        textScoreTime->setText(timeString);

    if (textClientName->getText() != newClientName)
        textClientName->setText(newClientName);

    while(textToAppend.size() > 0)
    {
        String* str = textToAppend.removeAndReturn(0);
        textReceivedCommmands->moveCaretToEnd();
        textReceivedCommmands->insertTextAtCaret(*str);
        delete str;
    }
}
