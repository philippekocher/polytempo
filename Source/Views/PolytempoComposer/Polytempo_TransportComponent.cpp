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

#include "Polytempo_TransportComponent.h"
#include "../../Scheduler/Polytempo_ScoreScheduler.h"
#include "../../Application/PolytempoComposer/Polytempo_ComposerApplication.h"
#include "../../Application/Polytempo_CommandIDs.h"
#include "../../Misc/Polytempo_Textbox.h"


Polytempo_TransportComponent::Polytempo_TransportComponent()
{
    startButton.reset(new Polytempo_Button(">", Polytempo_Button::buttonType_play));
    addAndMakeVisible(startButton.get());
    startButton->setConnectedEdges(Button::ConnectedOnRight);
    startButton->addListener(this);
    
    returnToLocatorButton.reset(new Polytempo_Button("|<"));
    addAndMakeVisible(returnToLocatorButton.get());
    returnToLocatorButton->setConnectedEdges(Button::ConnectedOnRight + Button::ConnectedOnLeft);
    returnToLocatorButton->setCommandToTrigger(&Polytempo_ComposerApplication::getCommandManager(), Polytempo_CommandIDs::returnToLoc, true);
    
    returnToZeroButton.reset(new Polytempo_Button("|<<"));
    addAndMakeVisible(returnToZeroButton.get());
    returnToZeroButton->setConnectedEdges(Button::ConnectedOnLeft);
    returnToZeroButton->setCommandToTrigger(&Polytempo_ComposerApplication::getCommandManager(), Polytempo_CommandIDs::returnToBeginning , true);
    
    
    addAndMakeVisible(timeTextbox = new Polytempo_Textbox(String()));
    timeTextbox->setFont(Font (24.0f, Font::bold));
    timeTextbox->addListener(this);

    addAndMakeVisible(tempoFactorTextbox = new Polytempo_Textbox(String()));
    tempoFactorTextbox->setFont(Font (24.0f, Font::bold));
    tempoFactorTextbox->setText("1.0", dontSendNotification);
    tempoFactorTextbox->addListener(this);
}

Polytempo_TransportComponent::~Polytempo_TransportComponent()
{
    startButton           = nullptr;
    returnToLocatorButton = nullptr;
    returnToZeroButton    = nullptr;
    
    deleteAllChildren();
}

void Polytempo_TransportComponent::paint (Graphics& g)
{
    g.fillAll(Colour(245,245,245));
    
    g.setColour(Colour(170,170,170));
    g.drawHorizontalLine(0, 0.0f, float(getWidth()));
    
    g.setColour(Colour(150,150,150));
    g.drawText("Tempo x", 320, int(getHeight() * 0.25), 100, 20, Justification::left);
}

void Polytempo_TransportComponent::resized()
{
    startButton->setBounds            (5,  int((getHeight() - 22) * 0.5), 31, 22);
    returnToLocatorButton->setBounds  (36, int((getHeight() - 22) * 0.5), 31, 22);
    returnToZeroButton->setBounds     (67, int((getHeight() - 22) * 0.5), 32, 22);
    
    timeTextbox->setBounds            (140, int((getHeight() - 28) * 0.5), 145, 29);
    tempoFactorTextbox->setBounds     (380, int((getHeight() - 28) * 0.5), 50, 29);
}

void Polytempo_TransportComponent::buttonClicked(Button* button)
{
    if(button == startButton.get()) Polytempo_ScoreScheduler::getInstance()->startStop();
}

void Polytempo_TransportComponent::editorShown (Label* label, TextEditor&)
{
    if(label == timeTextbox)             timeString        = label->getText();
    else if(label == tempoFactorTextbox) tempoFactorString = label->getText();
}

void Polytempo_TransportComponent::labelTextChanged (Label* label)
{
    if(label == timeTextbox)
    {
        Polytempo_ScoreScheduler::getInstance()->storeLocator(int(Polytempo_Textbox::stringToTime(label->getText()) * 1000.0f));
        Polytempo_ScoreScheduler::getInstance()->gotoTime(Polytempo_Event::makeEvent(eventType_GotoTime, Polytempo_Textbox::stringToTime(label->getText())));
    }
    else if(label == tempoFactorTextbox)
    {
        float num = (label->getText()).getFloatValue();
        if(num <= 0.0)
            label->setText(tempoFactorString, dontSendNotification);
        else
            Polytempo_ScoreScheduler::getInstance()->setTempoFactor(Polytempo_Event::makeEvent(eventType_TempoFactor, num));
    }
}

void Polytempo_TransportComponent::eventNotification(Polytempo_Event *event)
{
    if(event->getType() == eventType_Start)
    {
        startButton->setToggleState(true, dontSendNotification);
    }
    else if(event->getType() == eventType_Stop)
    {
        startButton->setToggleState(false, dontSendNotification);
    }
    else if(event->getType() == eventType_Tick)
    {
        // update the time with every tick
        timeTextbox->setText(Polytempo_Textbox::timeToString(event->getValue()), dontSendNotification);
    }
    else if(event->getType() == eventType_TempoFactor)
    {
        tempoFactorTextbox->setText(event->getValue(), dontSendNotification);
    }
    else if(event->getType() == eventType_DeleteAll)
    {
        Polytempo_ScoreScheduler::getInstance()->gotoTime(Polytempo_Event::makeEvent(eventType_GotoTime,0));
    }
}
