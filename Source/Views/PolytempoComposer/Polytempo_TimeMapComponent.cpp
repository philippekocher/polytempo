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

#include "../JuceLibraryCode/JuceHeader.h"
#include "Polytempo_TimeMapComponent.h"
#include "../../Misc/Polytempo_Globals.h"
#include "../../Preferences/Polytempo_StoredPreferences.h"


Polytempo_TimeMapComponent::Polytempo_TimeMapComponent()
{
    coordinateSystem.reset(new Polytempo_CoordinateSystem());
    addAndMakeVisible(coordinateSystem.get());
    coordinateSystem->setSynchronizedViewport(&timeRuler,0);
    coordinateSystem->setSynchronizedViewport(&positionRuler,1);
    
    timeMapCoordinateSystem.reset(new Polytempo_TimeMapCoordinateSystem(coordinateSystem.get()));
    coordinateSystem->setViewedComponent(timeMapCoordinateSystem.get(), false);
    
    addAndMakeVisible(timeRuler);
    addAndMakeVisible(positionRuler);
  
    timeRuler.setSynchronizedViewport(coordinateSystem.get(), 0);
    positionRuler.setSynchronizedViewport(coordinateSystem.get(), 1);
    
    coordinateSystem->setViewPositionProportionately(0.0, 1.0);

    Polytempo_StoredPreferences::getInstance()->getProps().addChangeListener(this);
    zoomX = float(Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("zoomX"));
    zoomY = float(Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("timeMapZoomY"));
}

Polytempo_TimeMapComponent::~Polytempo_TimeMapComponent()
{
    timeMapCoordinateSystem = nullptr;
    coordinateSystem = nullptr;
}

void Polytempo_TimeMapComponent::paint (Graphics& g)
{
    g.fillAll(Colour (245,245,245));
    
    g.setColour(Colours::grey);
    g.drawVerticalLine(getWidth() - 1, 0.0f, float(getHeight()));
}

void Polytempo_TimeMapComponent::resized()
{
    Rectangle<int> r (getLocalBounds());
    
    coordinateSystem->setBounds(r.withTrimmedBottom(40).withTrimmedLeft(70).withTrimmedRight(1));
    timeRuler.setBounds(r.removeFromBottom(40).withTrimmedLeft(70).withTrimmedRight(1));
    positionRuler.setBounds(r.removeFromLeft(70).withTrimmedLeft(5));
    
    // resize content while retaining scroll position
    float x = (coordinateSystem->getViewPositionX() - TIMEMAP_OFFSET) / zoomX;
    float y = (timeMapCoordinateSystem->getHeight() - coordinateSystem->getViewPositionY() - TIMEMAP_OFFSET - coordinateSystem->getMaximumVisibleHeight()) / zoomY;

    zoomX = float(Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("zoomX"));
    zoomY = float(Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("timeMapZoomY"));
    
    int width = int(relativeWidth * zoomX);
    if(width < coordinateSystem->getMaximumVisibleWidth())
        width = coordinateSystem->getMaximumVisibleWidth();

    int height = int(relativeHeight * zoomY) + 50;
    if(height < coordinateSystem->getMaximumVisibleHeight())
        height = coordinateSystem->getMaximumVisibleHeight();

    timeMapCoordinateSystem->setSizeAndZooms(width, height, zoomX, zoomY);
    positionRuler.setSizeAndZooms(0, height, zoomX, zoomY);
    timeRuler.setSizeAndZooms(width, 0, zoomX, zoomY);

    coordinateSystem->setViewPosition(TIMEMAP_OFFSET + int(x * zoomX), height - int(y * zoomY) - TIMEMAP_OFFSET - coordinateSystem->getMaximumVisibleHeight());
}

void Polytempo_TimeMapComponent::setRelativeSize(float maxTime, Rational maxPos)
{
    relativeWidth = maxTime;
    relativeHeight = maxPos;
    
    resized();
}

void Polytempo_TimeMapComponent::changeListenerCallback (ChangeBroadcaster*)
{
    resized();
    repaint();
}
