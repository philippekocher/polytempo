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

#include "Polytempo_AuxiliaryView.h"
#include "../../Scheduler/Polytempo_ScoreScheduler.h"
#include "../../Scheduler/Polytempo_EventDispatcher.h"
#include "../../Preferences/Polytempo_StoredPreferences.h"
#include "../../Network/Polytempo_NetworkSupervisor.h"
#include "../../Misc/Polytempo_Textbox.h"


Polytempo_AuxiliaryView::Polytempo_AuxiliaryView()
{
    addAndMakeVisible(markerTextbox = new Polytempo_Textbox("Marker", Polytempo_Textbox::textboxType_black));
    markerTextbox->setFont(Font (56.0f, Font::plain));
    markerTextbox->addListener(this);

    addAndMakeVisible(imageBackwards = new Polytempo_Button("<<", Polytempo_Button::buttonType_black));
    imageBackwards->setConnectedEdges(Button::ConnectedOnTop | Button::ConnectedOnRight);
    imageBackwards->addListener(this);
    
    addAndMakeVisible(markerBackwards = new Polytempo_Button("<", Polytempo_Button::buttonType_black));
    markerBackwards->setConnectedEdges(Button::ConnectedOnTop | Button::ConnectedOnRight | Button::ConnectedOnLeft);
    markerBackwards->addListener(this);
    
    addAndMakeVisible(markerForwards = new Polytempo_Button(">", Polytempo_Button::buttonType_black));
    markerForwards->setConnectedEdges(Button::ConnectedOnTop | Button::ConnectedOnRight | Button::ConnectedOnLeft);
    markerForwards->addListener(this);
    
    addAndMakeVisible(imageForwards = new Polytempo_Button(">>", Polytempo_Button::buttonType_black));
    imageForwards->setConnectedEdges(Button::ConnectedOnTop | Button::ConnectedOnLeft);
    imageForwards->addListener(this);
    
    addAndMakeVisible(timeTextbox = new Polytempo_Textbox("Time", Polytempo_Textbox::textboxType_black));
    timeTextbox->setFont(Font (24.0f, Font::plain));
    timeTextbox->addListener(this);
    
    addAndMakeVisible(tempoFactorTextbox = new Polytempo_Textbox("Tempo x", Polytempo_Textbox::textboxType_black));
    tempoFactorTextbox->setFont(Font (24.0000f, Font::plain));
    tempoFactorTextbox->addListener(this);
    tempoFactorTextbox->setText("1.0", dontSendNotification);
    
    addAndMakeVisible(peersTextbox = new Label());
    peersTextbox->setEditable(false, false, false);
    peersTextbox->setColour(Label::backgroundColourId, Colours::lightblue);

	addAndMakeVisible(annotationManager = new Polytempo_GraphicsAnnotationManagerView());
	annotationManager->setSize(100, 100);
}

Polytempo_AuxiliaryView::~Polytempo_AuxiliaryView()
{
    deleteAllChildren();
}

void Polytempo_AuxiliaryView::paint (Graphics& g)
{
    g.fillAll (Colours::white);   // clear the background
    
    AttributedString attributedPeers;
	attributedPeers.append("Network\n", Font(14.0f, Font::plain));
	attributedPeers.append(Polytempo_NetworkSupervisor::getInstance()->getLocalName(), Font(12.0f, Font::bold));
	attributedPeers.append(" \n", Font(22.0f, Font::plain));
	attributedPeers.append(Polytempo_NetworkSupervisor::getInstance()->getAdapterInfo()+"\n", Font(12.0f, Font::bold));
    
    HashMap < String, String >::Iterator it(*Polytempo_NetworkSupervisor::getInstance()->getPeers());
    while(it.next())
    {
		attributedPeers.append(it.getValue(), Font(12.0f, Font::plain));
		attributedPeers.append(" \n", Font(22.0f, Font::plain));
		attributedPeers.append(it.getKey()+"\n", Font(12.0f, Font::plain));
        
    }
    
    g.drawHorizontalLine(320, 0.0f, (float)getWidth());
	attributedPeers.draw(g, Rectangle<int>(10, 330, getWidth() - 20, 100).toFloat());

    g.setColour(Colours::grey);
    g.drawVerticalLine(0, 0.0f, (float)getHeight());
}

void Polytempo_AuxiliaryView::resized()
{
    int yPosition = 25;
    
    markerTextbox->setBounds(10, yPosition, getWidth() - 20, 66);
    yPosition +=66;
    
    int buttonWidth = (int)(getWidth() * 0.25f - 5.0f);
    int widthCorrection = getWidth() - 20 - buttonWidth * 4;
    imageBackwards->setBounds (10,                   yPosition, buttonWidth, 20);
    markerBackwards->setBounds(10 + buttonWidth * 1, yPosition, buttonWidth, 20);
    markerForwards->setBounds (10 + buttonWidth * 2, yPosition, buttonWidth, 20);
    imageForwards->setBounds  (10 + buttonWidth * 3, yPosition, buttonWidth + widthCorrection, 20);
    yPosition +=60;
    
    timeTextbox->setBounds(10, yPosition, getWidth() - 20, 34);
    yPosition +=80;
    
    tempoFactorTextbox->setBounds(10, yPosition, getWidth() - 20, 34);
    
	annotationManager->setBounds(10, getHeight() - annotationManager->getHeight() - 10, getWidth() - 20, annotationManager->getHeight());

    Polytempo_NetworkSupervisor::getInstance()->setComponent(this);
}

void Polytempo_AuxiliaryView::eventNotification(Polytempo_Event *event)
{
    if(!Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("showAuxiliaryView")) return;
    
    if(event->getType() == eventType_Marker)
    {
        markerTextbox->setText(event->getProperty("value"), dontSendNotification);
    }
    else if(event->getType() == eventType_TempoFactor)
    {
        tempoFactorTextbox->setFloat(event->getProperty("value"), dontSendNotification);
    }
    else if(event->getType() == eventType_Tick)
    {
       // update the time with every tick
        timeTextbox->setText(Polytempo_Textbox::timeToString(event->getValue()), dontSendNotification);
    }
}

void Polytempo_AuxiliaryView::editorShown (Label* label, TextEditor&)
{
    if     (label == markerTextbox)        markerString  = label->getText();
    else if(label == tempoFactorTextbox)   tempoFactor   = label->getText();
}

void Polytempo_AuxiliaryView::labelTextChanged(Label* label)
{
    if(label == markerTextbox)
    {
        if(!Polytempo_ScoreScheduler::getInstance()->gotoMarker(Polytempo_Event::makeEvent(eventType_GotoMarker, label->getTextValue().toString())))
            label->setText(markerString, dontSendNotification);
    }
    else if(label == timeTextbox)
    {
        Polytempo_ScoreScheduler::getInstance()->storeLocator(Polytempo_Textbox::stringToTime(label->getText()) * 1000.0f);
        Polytempo_EventDispatcher::getInstance()->broadcastEvent(Polytempo_Event::makeEvent(eventType_GotoTime, Polytempo_Textbox::stringToTime(label->getText())));
    }
    else if(label == tempoFactorTextbox)
    {
        float num = (label->getText()).getFloatValue();
        if(num <= 0.0)
            label->setText(tempoFactor, dontSendNotification);
        else
            Polytempo_EventDispatcher::getInstance()->broadcastEvent(Polytempo_Event::makeEvent(eventType_TempoFactor, num));
    }
}

// Button listener
void Polytempo_AuxiliaryView::buttonClicked(Button *button)
{
    if(button == markerBackwards)
        Polytempo_ScoreScheduler::getInstance()->skipToEvent(eventType_Marker, true);
    if(button == markerForwards)
        Polytempo_ScoreScheduler::getInstance()->skipToEvent(eventType_Marker, false);
    if(button == imageBackwards)
        Polytempo_ScoreScheduler::getInstance()->skipToEvent(eventType_Image, true);
    if(button == imageForwards)
        Polytempo_ScoreScheduler::getInstance()->skipToEvent(eventType_Image, false);
}

void Polytempo_AuxiliaryView::buttonStateChanged(Button*)
{}
