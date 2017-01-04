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

#include "../../JuceLibraryCode/JuceHeader.h"
#include "Polytempo_ComposerToolbarComponent.h"
#include "../../Application/PolytempoComposer/Polytempo_ComposerApplication.h"
#include "../../Application/Polytempo_CommandIDs.h"
#include "Polytempo_ComposerMainView.h"

Polytempo_ComposerToolbarComponent::Polytempo_ComposerToolbarComponent()
{
    addAndMakeVisible(showTimeMapButton = new Polytempo_Button("Time Map", Polytempo_Button::buttonType_toggle));
    showTimeMapButton->setConnectedEdges(Button::ConnectedOnRight);
    showTimeMapButton->addListener(this);
    
    addAndMakeVisible(showTempoMapButton = new Polytempo_Button("Tempo Map", Polytempo_Button::buttonType_toggle));
    showTempoMapButton->setConnectedEdges(Button::ConnectedOnLeft);
    showTempoMapButton->addListener(this);
    
    addAndMakeVisible(showBeatPatternListButton = new Polytempo_Button("Beat Patterns", Polytempo_Button::buttonType_toggle));
    showBeatPatternListButton->setConnectedEdges(Button::ConnectedOnRight);
    showBeatPatternListButton->addListener(this);
    
    addAndMakeVisible(showPointEditorButton = new Polytempo_Button("Control Points", Polytempo_Button::buttonType_toggle));
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
    g.drawHorizontalLine(getHeight() - 1, 0, getWidth());
}

void Polytempo_ComposerToolbarComponent::resized()
{
    showTimeMapButton->setBounds  (5,  (getHeight() - 18) * 0.5, 80, 18);
    showTempoMapButton->setBounds (85, (getHeight() - 18) * 0.5, 80, 18);
    
    showBeatPatternListButton->setBounds (getWidth() - 165, (getHeight() - 18) * 0.5, 80, 18);
    showPointEditorButton->setBounds     (getWidth() - 85,  (getHeight() - 18) * 0.5, 80, 18);
}

void Polytempo_ComposerToolbarComponent::buttonClicked(Button* button)
{
    if(button == showTimeMapButton)
    {
        Polytempo_ComposerApplication::getCommandManager().invokeDirectly(Polytempo_CommandIDs::showTimeMap, true);
        showTempoMapButton->setToggleState(false, dontSendNotification);
    }
    else if(button == showTempoMapButton)
    {
        Polytempo_ComposerApplication::getCommandManager().invokeDirectly(Polytempo_CommandIDs::showTempoMap, true);
        showTimeMapButton->setToggleState(false, dontSendNotification);
    }
    if(button == showBeatPatternListButton)
    {
        Polytempo_ComposerApplication::getCommandManager().invokeDirectly(Polytempo_CommandIDs::showPatternList, true);
        showPointEditorButton->setToggleState(false, dontSendNotification);
    }
    else if(button == showPointEditorButton)
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

