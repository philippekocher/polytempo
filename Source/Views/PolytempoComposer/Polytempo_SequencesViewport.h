#pragma once

#include "JuceHeader.h"
#include "../../Data/Polytempo_Composition.h"
#include "../../Misc/Polytempo_Button.h"

class Polytempo_ColourButton : public Button, ChangeListener
{
public:
    Polytempo_ColourButton(int index) : Button(String()), sequenceIndex(index)
    {}
    
    void paintButton(Graphics &g, bool /*mouseOver*/, bool /*buttonDown*/) override
    {
        Polytempo_Composition* composition = Polytempo_Composition::getInstance();
        
        Colour colour = composition->getSequence(sequenceIndex)->getColour();
        g.fillAll(colour);
    }
    
    void clicked() override
    {
        ColourSelector* colourSelector = new ColourSelector(ColourSelector::showColourspace);
        colourSelector->setName("background");
        colourSelector->setCurrentColour(Polytempo_Composition::getInstance()->getSequence(sequenceIndex)->getColour());
        colourSelector->addChangeListener(this);
        colourSelector->setColour(ColourSelector::backgroundColourId, Colours::transparentBlack);
        colourSelector->setSize(200, 200);
        
        CallOutBox::launchAsynchronously((std::unique_ptr<Component>)colourSelector, getScreenBounds(), nullptr);
    }
    
    void changeListenerCallback (ChangeBroadcaster* source) override
    {
        if (ColourSelector* cs = dynamic_cast<ColourSelector*> (source))
            Polytempo_Composition::getInstance()->getSequence(sequenceIndex)->setColour(cs->getCurrentColour());

        Polytempo_Composition::getInstance()->updateContent(); // repaint everything
    }
    
private:
    int sequenceIndex;
};


class Polytempo_SequenceControlComponent : public Component,
                                           public TextEditor::Listener,
                                           public Button::Listener
{
public:
    Polytempo_SequenceControlComponent(int);
    ~Polytempo_SequenceControlComponent();

    void paint (Graphics&);
    void resized();
    
    void showGraphicalSettings(bool);

    void mouseDown(const MouseEvent &);
    
    void textEditorTextChanged(TextEditor&);
    void textEditorReturnKeyPressed(TextEditor&);
    void textEditorEscapeKeyPressed(TextEditor&);
    
    void buttonClicked(Button*);

private:
    int sequenceIndex;
    TextEditor sequenceName;
    bool graphicalSettingsShown;
    
    std::unique_ptr<Polytempo_Button> playbackSettingsButton;
    std::unique_ptr<Polytempo_Button> graphicalSettingsButton;
    std::unique_ptr<Polytempo_Button> soloButton;
    std::unique_ptr<Polytempo_Button> muteButton;

    std::unique_ptr<Polytempo_ColourButton> colourSelector;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_SequenceControlComponent)
};


class Polytempo_SequencesViewport : public Viewport
{
public:
    Polytempo_SequencesViewport();
    ~Polytempo_SequencesViewport();

    void paint (Graphics&);
    void resized();

    void showGraphicalSettings(bool);

private:
    bool graphicalSettingsShown;
    std::unique_ptr<Component> viewedComponent;
    OwnedArray <Polytempo_SequenceControlComponent> sequenceControls;
};

class Polytempo_SequencesComponent : public Component
{
public:
    void mouseDown(const MouseEvent &);
    void showPopupMenu();

};
