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

#include "Polytempo_NetworkMainView.h"
#include "Polytempo_VisualMetro.h"
#include "Polytempo_GraphicsView.h"
#include "Polytempo_AuxiliaryView.h"
#include "../../Preferences/Polytempo_StoredPreferences.h"


Polytempo_NetworkMainView::Polytempo_NetworkMainView()
{
    Polytempo_StoredPreferences::getInstance()->getProps().addChangeListener(this);
    
    addAndMakeVisible(visualMetro = new Polytempo_VisualMetro());
    addAndMakeVisible(graphicsView = new Polytempo_GraphicsView());
    graphicsView->setAlwaysOnTop(true);
    addAndMakeVisible(auxiliaryView = new Polytempo_AuxiliaryView());
}

Polytempo_NetworkMainView::~Polytempo_NetworkMainView()
{
    deleteAllChildren();
}

#pragma mark -
#pragma mark graphics

void Polytempo_NetworkMainView::paint(Graphics& g)
{
    g.fillAll(Colours::white);
}

void Polytempo_NetworkMainView::resized()
{
    int stripWidth, stripLength, auxiliaryViewHeight, auxiliaryViewWidth;
    
    if(!Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("showAuxiliaryView"))
        auxiliaryViewHeight = auxiliaryViewWidth = 0;
    else
    {
        auxiliaryViewWidth = (int)(getWidth() * 0.12);
        auxiliaryViewWidth = auxiliaryViewWidth < 120 ? 120 : auxiliaryViewWidth > 200 ? 200 : auxiliaryViewWidth;
        
        auxiliaryViewHeight = 0;

    }

    auxiliaryView->setBounds(getWidth() - auxiliaryViewWidth,
                             0,
                             auxiliaryViewWidth,
                             getHeight());
    
    if(!Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("showVisualMetro"))
        stripWidth = 0;
    else
    {
        stripWidth = Polytempo_StoredPreferences::getInstance()->getProps().getIntValue("stripWidth");

        stripLength = getWidth() - auxiliaryViewWidth < getHeight() - auxiliaryViewHeight ? getWidth() - auxiliaryViewWidth : getHeight() - auxiliaryViewHeight;
    
    
        visualMetro->setBounds(0, 0, stripLength, stripLength);
        visualMetro->setWidth((float)stripWidth);
    }
    
    graphicsView->setBounds(stripWidth,
                            stripWidth,
                            getWidth() - stripWidth - auxiliaryViewWidth,
                            getHeight() - stripWidth - auxiliaryViewHeight);
    
}

void Polytempo_NetworkMainView::changeListenerCallback (ChangeBroadcaster* source)
{
    // settings have changed
    
    Colour fgColour = Colour::fromString(Polytempo_StoredPreferences::getInstance()->getProps().getValue("stripNormalColour"));
    Colour bgColour = Colour::fromString(Polytempo_StoredPreferences::getInstance()->getProps().getValue("stripBackgroundColour"));
    Colour frColour = Colour::fromString(Polytempo_StoredPreferences::getInstance()->getProps().getValue("stripFrameColour"));
    
    visualMetro->setForegroundColour(fgColour);
    visualMetro->setBackgroundColour(bgColour);
    visualMetro->setFrameColour(frColour);
    
    resized();
    repaint();
}