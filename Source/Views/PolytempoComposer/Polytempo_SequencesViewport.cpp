/* ==============================================================================
 
 This file is part of the POLYTEMPO software package.
 Copyright (c) 2016 - Zurich University of the Arts,
 ICST Institute for Computer Music and Sound Technology
 http://www.icst.net
 
 Author: Philippe Kocher
 
 POLYTEMPO is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 POLYTEMPO is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this software. If not, see <http://www.gnu.org/licenses/>.
 
 ============================================================================== */

//#include "../JuceLibraryCode/JuceHeader.h"
#include "Polytempo_SequencesViewport.h"
#include "Polytempo_SequencePlaybackSettings.h"
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
            viewedComponent->addAndMakeVisible(sequenceControl);
        }
    }

    int width = composition->getNumberOfSequences() * 100 > getWidth() ? composition->getNumberOfSequences() * 100 : getWidth();
    viewedComponent->setBounds(0, 0, width, getHeight());
    
    int height = getHorizontalScrollBar().isVisible() ? getHeight()-getScrollBarThickness() : getHeight();
    for(int i=0;i<sequenceControls.size();i++)
        sequenceControls[i]->setBounds(i*100, 0, 100, height);
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

    settingsButton.reset(new Polytempo_Button("Playback Settings"));
    addAndMakeVisible(settingsButton.get());
    settingsButton->addListener(this);
    
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
    
    if(composition->getSelectedSequenceIndex() == sequenceIndex)
    {
//        sequenceName.setColour(TextEditor::textColourId, Colours::black);
    }
    else
    {
//        sequenceName.setColour(TextEditor::textColourId, Colour(200,200,200));
    }
}

void Polytempo_SequenceControlComponent::resized()
{
    colourSelector->setBounds   (10, 15, 80, 10);
    
    sequenceName.setBounds      (10, 25, 80, 16);
    
    settingsButton->setBounds   (10, getHeight() -  52, 80, 16);

    soloButton->setBounds       (10, getHeight() -  32, 40, 16);
    muteButton->setBounds       (51, getHeight() -  32, 39, 16);
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

void Polytempo_SequenceControlComponent::buttonClicked(Button* button)
{
    bool buttonState = button->getToggleState();
 
    if(button == settingsButton.get())
    {
        button->setToggleState(true, dontSendNotification);
        Polytempo_SequencePlaybackSettings::show(Polytempo_Composition::getInstance()->getSequence(sequenceIndex));
        button->setToggleState(false, dontSendNotification);
    }
    else if(button == muteButton.get())
    {
        Polytempo_Composition::getInstance()->getSequence(sequenceIndex)->mute = buttonState;
    }
}
