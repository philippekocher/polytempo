#include "Polytempo_SequencesViewport.h"
#include "Polytempo_SequencePlaybackSettings.h"
#include "Polytempo_SequenceGraphicalSettings.h"
#include "../../Data/Polytempo_Composition.h"
#include "../../Application/PolytempoComposer/Polytempo_ComposerApplication.h"
#include "../../Application/Polytempo_CommandIDs.h"


Polytempo_SequencesViewport::Polytempo_SequencesViewport()
{
    setScrollBarsShown(false, true, false, true);
    
    viewedComponent.reset(new Polytempo_SequencesComponent());
    setViewedComponent(viewedComponent.get(), false);
    setViewPositionProportionately(0.0, 0.0);
    
    viewedComponent->setMouseClickGrabsKeyboardFocus(false);
}

Polytempo_SequencesViewport::~Polytempo_SequencesViewport()
{
    viewedComponent = nullptr;
}

void Polytempo_SequencesViewport::paint(Graphics& g)
{
    g.fillAll(Colour(245,245,245));

    g.setColour(Colour(170,170,170));
    g.drawHorizontalLine(0, 0.0f, float(getWidth()));
    
    Polytempo_Composition* composition = Polytempo_Composition::getInstance();
    if(sequenceControls.size() != composition->getNumberOfSequences()) resized();
        
}

void Polytempo_SequencesViewport::resized()
{
    Polytempo_Composition* composition = Polytempo_Composition::getInstance();
    
    if(sequenceControls.size() != composition->getNumberOfSequences())
    {
        viewedComponent->removeAllChildren();
        sequenceControls.clear();
        
        Polytempo_Sequence* sequence;
        Polytempo_SequenceControlComponent* sequenceControl;
        int i = -1;
        while((sequence = composition->getSequence(++i)) != nullptr)
        {
            sequenceControls.add(sequenceControl = new Polytempo_SequenceControlComponent(i));
            sequenceControl->showGraphicalSettings(graphicalSettingsShown);
            viewedComponent->addAndMakeVisible(sequenceControl);
        }
    }

    int width = composition->getNumberOfSequences() * 100 > getWidth() ? composition->getNumberOfSequences() * 100 : getWidth();
    viewedComponent->setBounds(0, 0, width, getHeight());
    
    int height = getHorizontalScrollBar().isVisible() ? getHeight()-getScrollBarThickness() : getHeight();
    for(int i=0;i<sequenceControls.size();i++)
        sequenceControls[i]->setBounds(i*100, 0, 100, height);
}

void Polytempo_SequencesViewport::showGraphicalSettings(bool show)
{
   graphicalSettingsShown = show;
   for(Polytempo_SequenceControlComponent *sequenceControl : sequenceControls)
    {
        sequenceControl->showGraphicalSettings(show);
    }
}


void Polytempo_SequencesComponent::mouseDown(const MouseEvent &event)
{
    if(event.mods.isPopupMenu()) showPopupMenu();
}

void Polytempo_SequencesComponent::showPopupMenu()
{
    ApplicationCommandManager* commandManager = &Polytempo_ComposerApplication::getCommandManager();
    PopupMenu m;
    
    m.addCommandItem(commandManager, Polytempo_CommandIDs::addSequence);
    m.addCommandItem(commandManager, Polytempo_CommandIDs::removeSequence);
    
    m.show();
}



Polytempo_SequenceControlComponent::Polytempo_SequenceControlComponent(int i) : sequenceIndex(i)
{
    setMouseClickGrabsKeyboardFocus(false);

    colourSelector.reset(new Polytempo_ColourButton(sequenceIndex));
    addAndMakeVisible(colourSelector.get());
    colourSelector->setWantsKeyboardFocus(false);
    
    addAndMakeVisible(sequenceName);
    sequenceName.addListener(this);
    sequenceName.setFont(11);
    sequenceName.setColour(TextEditor::backgroundColourId, Colours::transparentBlack);
    sequenceName.setColour(TextEditor::highlightedTextColourId, Colours::white);
    sequenceName.setColour(TextEditor::focusedOutlineColourId, Colours::white);
    sequenceName.setText(Polytempo_Composition::getInstance()->getSequence(sequenceIndex)->getName());

    playbackSettingsButton.reset(new Polytempo_Button("Playback Settings"));
    addAndMakeVisible(playbackSettingsButton.get());
    playbackSettingsButton->addListener(this);
    
    graphicalSettingsButton.reset(new Polytempo_Button("Graphical Settings"));
    addAndMakeVisible(graphicalSettingsButton.get());
    graphicalSettingsButton->addListener(this);
    
    soloButton.reset(new Polytempo_Button("Solo", Polytempo_Button::buttonType_toggleYellow));
    addAndMakeVisible(soloButton.get());
    soloButton->addListener(this);
    
    muteButton.reset(new Polytempo_Button("Mute", Polytempo_Button::buttonType_toggleYellow));
    addAndMakeVisible(muteButton.get());
    muteButton->addListener(this);
}

Polytempo_SequenceControlComponent::~Polytempo_SequenceControlComponent()
{
    colourSelector = nullptr;
    muteButton = nullptr;
    soloButton = nullptr;
}

void Polytempo_SequenceControlComponent::paint (Graphics& g)
{
    Polytempo_Composition* composition = Polytempo_Composition::getInstance();

    if(composition->getSelectedSequenceIndex() == sequenceIndex)
        g.setColour(Colour(0,0,0));
    else
        g.setColour(Colour(200,200,200));

    g.drawRoundedRectangle(4.0f, 9.0f, 94.0f, float(getHeight() - 18), 4.0f, 1.0f);

    if(composition->getSelectedSequenceIndex() == sequenceIndex)
        g.setColour(Colour(225,225,225));
    else
        g.setColour(Colour(235,235,235));

    g.fillRoundedRectangle(5.0f, 10.0f, 92.0f, float(getHeight() - 20), 4.0f);
}

void Polytempo_SequenceControlComponent::resized()
{
    colourSelector->setBounds   (10, 15, 80, 10);
    
    sequenceName.setBounds      (10, 25, 80, 16);
    
    if(graphicalSettingsShown)
        graphicalSettingsButton->setBounds(10, getHeight() -  52, 80, 16);
    else
       playbackSettingsButton->setBounds(10, getHeight() -  52, 80, 16);
    
    soloButton->setBounds       (10, getHeight() -  32, 40, 16);
    muteButton->setBounds       (51, getHeight() -  32, 39, 16);
}

void Polytempo_SequenceControlComponent::showGraphicalSettings(bool show)
{
    graphicalSettingsShown = show;
    resized();
}

void Polytempo_SequenceControlComponent::mouseDown(const MouseEvent &event)
{
    if(event.mods.isPopupMenu())
    {
        ((Polytempo_SequencesComponent*)getParentComponent())->showPopupMenu();
    }
    else
    {
        Polytempo_Composition* composition = Polytempo_Composition::getInstance();
        composition->setSelectedSequenceIndex(sequenceIndex);
    
        composition->updateContent(); // repaint everything
    }
}

void Polytempo_SequenceControlComponent::textEditorTextChanged(TextEditor &editor)
{
    Polytempo_Composition::getInstance()->getSequence(sequenceIndex)->setName(editor.getText());
}

void Polytempo_SequenceControlComponent::textEditorReturnKeyPressed(TextEditor &editor)
{
    editor.giveAwayKeyboardFocus();
}

void Polytempo_SequenceControlComponent::textEditorEscapeKeyPressed(TextEditor &editor)
{
    editor.giveAwayKeyboardFocus();
}

void Polytempo_SequenceControlComponent::buttonClicked(Button* button)
{
    bool buttonState = button->getToggleState();
 
    if(button == playbackSettingsButton.get())
    {
        button->setToggleState(true, dontSendNotification);
        Polytempo_SequencePlaybackSettings::show(Polytempo_Composition::getInstance()->getSequence(sequenceIndex));
        button->setToggleState(false, dontSendNotification);
    }
    else if(button == graphicalSettingsButton.get())
    {
        button->setToggleState(true, dontSendNotification);
        Polytempo_SequenceGraphicalSettings::show(Polytempo_Composition::getInstance()->getSequence(sequenceIndex));
        button->setToggleState(false, dontSendNotification);
    }
    else if(button == soloButton.get())
    {
        Polytempo_Composition::getInstance()->getSequence(sequenceIndex)->solo = buttonState;
        Polytempo_Composition::getInstance()->getSequence(sequenceIndex)->mute = false;
        muteButton->setToggleState(false, dontSendNotification);
    }
    else if(button == muteButton.get())
    {
        Polytempo_Composition::getInstance()->getSequence(sequenceIndex)->mute = buttonState;
        Polytempo_Composition::getInstance()->getSequence(sequenceIndex)->solo = false;
        soloButton->setToggleState(false, dontSendNotification);
    }
}
