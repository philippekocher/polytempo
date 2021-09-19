#include "MainComponent.h"

#ifdef STATIC_LIBRARY_MODE
#include "../../../Source/Library/Polytempo_LibApi.h"
#define POLYTEMPOCALL(x) polytempo_##x
#define POLYTEMPO_RELEASE polytempo_release()
#else
#include "../../../Source/Library/Polytempo_LibMain.h"
#define POLYTEMPOCALL(x) Polytempo_LibMain::current()->x
#define POLYTEMPO_RELEASE Polytempo_LibMain::release()
#endif

//==============================================================================
MainComponent::MainComponent()
{
    auto applicationName = JUCEApplication::getInstance()->getApplicationName().toStdString();

    // In
    groupIn.reset(new GroupComponent("In", "In"));
    addAndMakeVisible(groupIn.get());

    toggleMaster.reset(new ToggleButton("Master"));
    toggleMaster->addListener(this);
    addAndMakeVisible(toggleMaster.get());

    labelClientName.reset(new Label("ClientName", "Client Name"));
    addAndMakeVisible(labelClientName.get());

    textClientName.reset(new TextEditor());
    textClientName->setText(applicationName);
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

    toggleConnectedToMaster.reset(new ToggleButton("Connected to Master"));
    toggleConnectedToMaster->setEnabled(false);
    addAndMakeVisible(toggleConnectedToMaster.get());

    labelNetworkStatus.reset(new Label("Network Status", "Network Status"));
    addAndMakeVisible(labelNetworkStatus.get());

    textNetworkStatus.reset(new TextEditor());
    textNetworkStatus->setReadOnly(true);
    addAndMakeVisible(textNetworkStatus.get());

    labelTime.reset(new Label("SyncTime", "Sync Time"));
    addAndMakeVisible(labelTime.get());

    textTime.reset(new TextEditor());
    textTime->setReadOnly(true);
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

    POLYTEMPOCALL(initialize(47522, false));
    POLYTEMPOCALL(setClientName(applicationName));
    POLYTEMPOCALL(registerEventCallback(this));
    POLYTEMPOCALL(registerTickCallback(this));

    startTimerHz(5);
}

MainComponent::~MainComponent()
{
    POLYTEMPO_RELEASE;
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

    groupIn->setBounds(0, 0, getWidth(), 3 * rowHeight + 4 * border);
    toggleMaster->setBounds(groupIn->getX() + border, groupIn->getY() + border, groupIn->getWidth() - 2 * border, rowHeight);

    labelClientName->setBounds(groupIn->getX() + border, groupIn->getY() + 2 * border + rowHeight, labelWidth, rowHeight);
    textClientName->setBounds(groupIn->getX() + labelWidth + 2 * border, groupIn->getY() + 2 * border + rowHeight, groupIn->getWidth() - labelWidth - 4 * border - buttonWidth, rowHeight);
    btnSetClientName->setBounds(groupIn->getRight() - 2 * border - buttonWidth, groupIn->getY() + 2 * border + rowHeight, buttonWidth, rowHeight);

    labelCommand->setBounds(groupIn->getX() + border, groupIn->getY() + 3 * border + 2 * rowHeight, labelWidth, rowHeight);
    textCommand->setBounds(groupIn->getX() + labelWidth + 2 * border, groupIn->getY() + 3 * border + 2 * rowHeight, groupIn->getWidth() - labelWidth - 4 * border - buttonWidth, rowHeight);
    btnSendCommand->setBounds(groupIn->getRight() - 2 * border - buttonWidth, groupIn->getY() + 3 * border + 2 * rowHeight, buttonWidth, rowHeight);

    groupOut->setBounds(0, groupIn->getBottom(), getWidth(), getHeight() - groupIn->getBottom());
    toggleConnectedToMaster->setBounds(groupOut->getX() + border, groupOut->getY() + border, groupOut->getWidth() - 2 * border, rowHeight);

    labelNetworkStatus->setBounds(groupOut->getX() + border, groupOut->getY() + 2 * border + rowHeight, labelWidth, rowHeight);
    textNetworkStatus->setBounds(groupOut->getX() + labelWidth + 2 * border, groupOut->getY() + 2 * border + rowHeight, groupOut->getWidth() - labelWidth - 3 * border, rowHeight);

    labelTime->setBounds(groupOut->getX() + border, groupOut->getY() + 3 * border + 2 * rowHeight, labelWidth, rowHeight);
    textTime->setBounds(groupOut->getX() + labelWidth + 2 * border, groupOut->getY() + 3 * border + 2 * rowHeight, groupOut->getWidth() - labelWidth - 3 * border, rowHeight);

    labelScoreTime->setBounds(groupOut->getX() + border, groupOut->getY() + 4 * border + 3 * rowHeight, labelWidth, rowHeight);
    textScoreTime->setBounds(groupOut->getX() + labelWidth + 2 * border, groupOut->getY() + 4 * border + 3 * rowHeight, groupOut->getWidth() - labelWidth - 3 * border, rowHeight);

    labelReceivedCommands->setBounds(groupOut->getX() + border, groupOut->getY() + 5 * border + 4 * rowHeight, groupOut->getWidth() - 2 * border, rowHeight);
    textReceivedCommmands->setBounds(groupOut->getX() + border, groupOut->getY() + 5 * border + 5 * rowHeight, groupOut->getWidth() - 2 * border, groupOut->getHeight() - 6 * border - 5 * rowHeight);
}

void MainComponent::timerCallback()
{
    uint32_t t;
    bool ok = POLYTEMPOCALL(getTime(&t) == TIME_SYNC_OK);
    textTime->setText(ok ? String(t * 0.001, 3) : "-");
    textTime->setColour(TextEditor::backgroundColourId, ok ? Colours::darkgreen : Colours::darkred);
}

void MainComponent::buttonClicked(Button* btn)
{
    if (btn == toggleMaster.get())
    {
        POLYTEMPOCALL(toggleMaster(toggleMaster->getToggleState()));
    }
    else if(btn == btnSetClientName.get())
    {
        POLYTEMPOCALL(setClientName(textClientName->getText().toStdString()));
    }
    else if(btn == btnSendCommand.get())
    {
        POLYTEMPOCALL(sendEvent(textCommand->getText().toStdString()));
    }
}

void MainComponent::processEvent(std::string const& message)
{
    const MessageManagerLock mmLock;

    uint32_t t;
    const String timeString = (POLYTEMPOCALL(getTime(&t)) == TIME_SYNC_OK) ? String(t * 0.001, 3) : "-";

    textReceivedCommmands->moveCaretToEnd();
    textReceivedCommmands->insertTextAtCaret(timeString + "\t " + message + NewLine::getDefault());
}

void MainComponent::processTick(double tick)
{
    textScoreTime->setText(String(tick, 3));
}
