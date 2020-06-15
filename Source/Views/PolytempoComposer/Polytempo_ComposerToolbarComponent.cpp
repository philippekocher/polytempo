#include "Polytempo_ComposerToolbarComponent.h"
#include "../../Application/PolytempoComposer/Polytempo_ComposerApplication.h"
#include "../../Application/Polytempo_CommandIDs.h"
#include "../../Misc/Polytempo_TempoMeasurement.h"
#include "Polytempo_ComposerMainView.h"


Polytempo_ComposerToolbarComponent::Polytempo_ComposerToolbarComponent()
{
    showTimeMapButton.reset(new Polytempo_Button("Time Map", Polytempo_Button::buttonType_toggle));
    addAndMakeVisible(showTimeMapButton.get());
    showTimeMapButton->setConnectedEdges(Button::ConnectedOnRight);
    showTimeMapButton->addListener(this);
    
    showTempoMapButton.reset(new Polytempo_Button("Tempo Map", Polytempo_Button::buttonType_toggle));
    addAndMakeVisible(showTempoMapButton.get());
    showTempoMapButton->setConnectedEdges(Button::ConnectedOnLeft);
    showTempoMapButton->addListener(this);
    
    showBeatPatternListButton.reset(new Polytempo_Button("Beat Patterns", Polytempo_Button::buttonType_toggle));
    addAndMakeVisible(showBeatPatternListButton.get());
    showBeatPatternListButton->setConnectedEdges(Button::ConnectedOnRight);
    showBeatPatternListButton->addListener(this);
    
    showPointEditorButton.reset(new Polytempo_Button("Control Points", Polytempo_Button::buttonType_toggle));
    addAndMakeVisible(showPointEditorButton.get());
    showPointEditorButton->setConnectedEdges(Button::ConnectedOnLeft);
    showPointEditorButton->addListener(this);
}

Polytempo_ComposerToolbarComponent::~Polytempo_ComposerToolbarComponent()
{
    showTimeMapButton  = nullptr;
    showTempoMapButton = nullptr;
    showBeatPatternListButton = nullptr;
    showPointEditorButton = nullptr;
}

void Polytempo_ComposerToolbarComponent::paint (Graphics& g)
{
    g.fillAll(Colour(245,245,245));
    
    g.setColour(Colour(170,170,170));
    g.drawHorizontalLine(getHeight() - 1, 0.0f, float(getWidth()));
    
    g.drawFittedText("Tempo Measurement: "+Polytempo_TempoMeasurement::getMeasurementAsString(), int(getWidth() * 0.5) - 50, 8, 100, 20, Justification::centred, 2);
}

void Polytempo_ComposerToolbarComponent::resized()
{
    showTimeMapButton->setBounds  (5,  int((getHeight() - 18) * 0.5), 80, 18);
    showTempoMapButton->setBounds (85, int((getHeight() - 18) * 0.5), 80, 18);
    
    showBeatPatternListButton->setBounds (getWidth() - 165, int((getHeight() - 18) * 0.5), 80, 18);
    showPointEditorButton->setBounds     (getWidth() - 85,  int((getHeight() - 18) * 0.5), 80, 18);
}

void Polytempo_ComposerToolbarComponent::buttonClicked(Button* button)
{
    if(button == showTimeMapButton.get())
    {
        Polytempo_ComposerApplication::getCommandManager().invokeDirectly(Polytempo_CommandIDs::showTimeMap, true);
        showTempoMapButton->setToggleState(false, dontSendNotification);
    }
    else if(button == showTempoMapButton.get())
    {
        Polytempo_ComposerApplication::getCommandManager().invokeDirectly(Polytempo_CommandIDs::showTempoMap, true);
        showTimeMapButton->setToggleState(false, dontSendNotification);
    }
    if(button == showBeatPatternListButton.get())
    {
        Polytempo_ComposerApplication::getCommandManager().invokeDirectly(Polytempo_CommandIDs::showPatternList, true);
        showPointEditorButton->setToggleState(false, dontSendNotification);
    }
    else if(button == showPointEditorButton.get())
    {
        Polytempo_ComposerApplication::getCommandManager().invokeDirectly(Polytempo_CommandIDs::showPointList, true);
        showBeatPatternListButton->setToggleState(false, dontSendNotification);
    }
   
    getParentComponent()->resized();
}

void Polytempo_ComposerToolbarComponent::setComponentTypes(int left, int right)
{
    showTimeMapButton->setToggleState (left == Polytempo_ComposerMainView::componentType_TimeMap, dontSendNotification);
    showTempoMapButton->setToggleState(left == Polytempo_ComposerMainView::componentType_TempoMap, dontSendNotification);
    
    showBeatPatternListButton->setToggleState(right == Polytempo_ComposerMainView::componentType_PatternList, dontSendNotification);
    showPointEditorButton->setToggleState(right == Polytempo_ComposerMainView::componentType_PointList, dontSendNotification);
}
