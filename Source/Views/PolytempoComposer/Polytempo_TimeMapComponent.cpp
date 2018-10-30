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


Polytempo_TimeMapComponent::Polytempo_TimeMapComponent()
{
    addAndMakeVisible(coordinateSystem = new Polytempo_CoordinateSystem());

    coordinateSystem->setSynchronizedViewport(&timeRuler,0);
    coordinateSystem->setSynchronizedViewport(&positionRuler,1);
    
    timeMapCoordinateSystem = new Polytempo_TimeMapCoordinateSystem(coordinateSystem);
    timeMapCoordinateSystem->setBounds(Rectangle<int> (3800, 3800));
    coordinateSystem->setViewedComponent(timeMapCoordinateSystem, false);
    
    addAndMakeVisible(timeRuler);
    addAndMakeVisible(positionRuler);
  
    timeRuler.setSynchronizedViewport(coordinateSystem,0);
    positionRuler.setSynchronizedViewport(coordinateSystem,1);
    
    coordinateSystem->setViewPositionProportionately(0.0, 1.0);
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
    g.drawVerticalLine(getWidth() - 1, 0, getHeight());
}

void Polytempo_TimeMapComponent::resized()
{
    Rectangle<int> r (getLocalBounds());
    
    coordinateSystem->setBounds(r.withTrimmedBottom(40).withTrimmedLeft(70).withTrimmedRight(1));
    timeRuler.setBounds(r.removeFromBottom(40).withTrimmedLeft(70).withTrimmedRight(1));
    positionRuler.setBounds(r.removeFromLeft(70).withTrimmedLeft(5));
}
