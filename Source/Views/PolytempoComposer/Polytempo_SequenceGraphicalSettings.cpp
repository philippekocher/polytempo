#include "Polytempo_SequenceGraphicalSettings.h"
#include "../../Application/PolytempoComposer/Polytempo_ComposerApplication.h"
#include "../../Data/Polytempo_Sequence.h"


Polytempo_SequenceGraphicalSettings::Polytempo_SequenceGraphicalSettings(Polytempo_Sequence* theSequence)
:sequence(theSequence)
{
    addAndMakeVisible(showNameButton = new ToggleButton("Show Name"));
    showNameButton->setToggleState(sequence->showName, dontSendNotification);
    showNameButton->addListener(this);
    
    addAndMakeVisible(staveOffsetTextbox = new Polytempo_Textbox("Stave Offset"));
    staveOffsetTextbox->setText(String(sequence->staveOffset), dontSendNotification);
    staveOffsetTextbox->setEnabled(true);
    staveOffsetTextbox->setInputRestrictions(0, "0123456789");
    staveOffsetTextbox->addListener(this);

    addAndMakeVisible(numberOfStavesTextbox = new Polytempo_Textbox("Number of Staves"));
    numberOfStavesTextbox->setText(String(sequence->numberOfStaves), dontSendNotification);
    numberOfStavesTextbox->setEnabled(true);
    numberOfStavesTextbox->setInputRestrictions(0, "0123456789", 1, 32);
    numberOfStavesTextbox->addListener(this);

    addAndMakeVisible(secondaryStaveOffsetTextbox = new Polytempo_Textbox("Secondary Stave Offset"));
    secondaryStaveOffsetTextbox->setText(String(sequence->secondaryStaveOffset), dontSendNotification);
    secondaryStaveOffsetTextbox->setEnabled(sequence->numberOfStaves > 1);
    secondaryStaveOffsetTextbox->setInputRestrictions(0, "0123456789");
    secondaryStaveOffsetTextbox->addListener(this);

    addAndMakeVisible(numberOfLinesTextbox = new Polytempo_Textbox("Number of Lines"));
    numberOfLinesTextbox->setText(String(sequence->numberOfLines), dontSendNotification);
    numberOfLinesTextbox->setEnabled(true);
    numberOfLinesTextbox->setInputRestrictions(0, "0123456789", 1, 32);
    numberOfLinesTextbox->addListener(this);

    addAndMakeVisible(lineOffsetTextbox = new Polytempo_Textbox("Line Offset"));
    lineOffsetTextbox->setText(String(sequence->lineOffset), dontSendNotification);
    lineOffsetTextbox->setEnabled(true);
    lineOffsetTextbox->setInputRestrictions(0, "0123456789");
    lineOffsetTextbox->addListener(this);
}

Polytempo_SequenceGraphicalSettings::~Polytempo_SequenceGraphicalSettings()
{
    deleteAllChildren();
}

void Polytempo_SequenceGraphicalSettings::resized()
{
    showNameButton->setBounds         (20, 20,  int(getWidth() *0.25) - 30, 20);
    staveOffsetTextbox->setBounds     (20, 80,  int(getWidth() * 0.25) - 30, 20);
    
    numberOfStavesTextbox->setBounds      (20, 140, int(getWidth() *0.25) - 30, 20);
    secondaryStaveOffsetTextbox->setBounds(20, 180, int(getWidth() *0.25) - 30, 20);
    
    numberOfLinesTextbox->setBounds   (20, 240, int(getWidth() * 0.25) - 30, 20);
    lineOffsetTextbox->setBounds      (20, 280, int(getWidth() * 0.25) - 30, 20);
}

void Polytempo_SequenceGraphicalSettings::paint (Graphics& g)
{
    g.fillAll (Colours::white);
}

void Polytempo_SequenceGraphicalSettings::buttonClicked(Button* button)
{
    bool buttonState = button->getToggleState();
    
    if(button == showNameButton)
        sequence->showName = buttonState;

    // update window
    Polytempo_ComposerApplication* const app = dynamic_cast<Polytempo_ComposerApplication*>(JUCEApplication::getInstance());
    app->getDocumentWindow().getContentComponent()->resized();
}

void Polytempo_SequenceGraphicalSettings::labelTextChanged(Label* textbox)
{
    String labelText = textbox->getText();
    
    if(textbox == staveOffsetTextbox)
        sequence->staveOffset = labelText.getIntValue();
    else if(textbox == numberOfStavesTextbox)
        sequence->numberOfStaves = labelText.getIntValue();
    else if(textbox == secondaryStaveOffsetTextbox)
        sequence->secondaryStaveOffset = labelText.getIntValue();
    else if(textbox == numberOfLinesTextbox)
        sequence->numberOfLines = labelText.getIntValue();
    else if(textbox == lineOffsetTextbox)
        sequence->lineOffset = labelText.getIntValue();

    // update window
    secondaryStaveOffsetTextbox->setEnabled(sequence->numberOfStaves > 1);
    Polytempo_ComposerApplication* const app = dynamic_cast<Polytempo_ComposerApplication*>(JUCEApplication::getInstance());
    app->getDocumentWindow().getContentComponent()->resized();
}

void Polytempo_SequenceGraphicalSettings::show(Polytempo_Sequence* sequence)
{
    Polytempo_ComposerApplication* const app = dynamic_cast<Polytempo_ComposerApplication*>(JUCEApplication::getInstance());
    Polytempo_SequenceGraphicalSettings settings(sequence);
    
    settings.setBounds(0,0,500,590);
    
    DialogWindow::LaunchOptions options;
    options.dialogTitle = "Graphical Settings ("+sequence->getName()+")";
    options.content.setNonOwned(&settings);
    options.componentToCentreAround = &app->getMainView();
    options.useNativeTitleBar = true;
    options.escapeKeyTriggersCloseButton = true;
    options.resizable = false;
    
    options.runModal();
}
