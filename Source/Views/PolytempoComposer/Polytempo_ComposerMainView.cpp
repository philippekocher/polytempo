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

#include "Polytempo_ComposerMainView.h"

#define TOOLBAR_HEIGHT 36
#define TRANSPORT_HEIGHT 40
#define SEQUENCES_HEIGHT 120


Polytempo_ComposerMainView::Polytempo_ComposerMainView()
{
    toolbarComponent.reset(new Polytempo_ComposerToolbarComponent());
    addAndMakeVisible(toolbarComponent.get());
    
    transportComponent.reset(new Polytempo_TransportComponent());
    addAndMakeVisible(transportComponent.get());
    
    addAndMakeVisible(sequencesViewport);
    
    addAndMakeVisible(leftComponent);
    addAndMakeVisible(rightComponent);
<<<<<<< HEAD

    resizerBar.reset(new StretchableLayoutResizerBar(&stretchableManager, 1, true));
    addAndMakeVisible(resizerBar.get());
    
    timeMapComponent.reset(new Polytempo_TimeMapComponent());
    tempoMapComponent.reset(new Polytempo_TempoMapComponent());
=======
    
    resizerBar.reset(new StretchableLayoutResizerBar(&stretchableManager, 1, true));
    addAndMakeVisible(resizerBar.get());
>>>>>>> master
    
    timeMapComponent.reset(new Polytempo_TimeMapComponent());
    tempoMapComponent.reset(new Polytempo_TempoMapComponent());
    
    stretchableManager.setItemLayout (1,1,1,1);     // hard limit to 1 pixel
    setSize(1, 1);
}

Polytempo_ComposerMainView::~Polytempo_ComposerMainView()
{
    timeMapComponent  = nullptr;
    tempoMapComponent = nullptr;
}

void Polytempo_ComposerMainView::paint (Graphics& g)
{}


void Polytempo_ComposerMainView::resized()
{
    Rectangle<int> r (getLocalBounds());
    
    
    // this will position the components and the resizer bar
    Component* comps[] = { &leftComponent, resizerBar.get(), &rightComponent };
    stretchableManager.layOutComponents(comps, 3,
                                         0, TOOLBAR_HEIGHT, r.getWidth(), r.getHeight() - TOOLBAR_HEIGHT - TRANSPORT_HEIGHT - SEQUENCES_HEIGHT,
                                         false, true);

    timeMapComponent->setBounds(leftComponent.getLocalBounds());
    tempoMapComponent->setBounds(leftComponent.getLocalBounds());

    pointListComponent.setBounds(rightComponent.getLocalBounds());
    beatPatternListComponent.setBounds(rightComponent.getLocalBounds());
    
    
    toolbarComponent->setBounds(r.withHeight(TOOLBAR_HEIGHT));
    transportComponent->setBounds(r.withY(r.getHeight() - SEQUENCES_HEIGHT - TRANSPORT_HEIGHT).withHeight(TRANSPORT_HEIGHT));
    sequencesViewport.setBounds(r.removeFromBottom(SEQUENCES_HEIGHT));
    sequencesViewport.showGraphicalSettings(false);
}

void Polytempo_ComposerMainView::childBoundsChanged(Component* child)
{
    if (child == &leftComponent)
        resized();
}

String Polytempo_ComposerMainView::getComponentStateAsString()
{
    resizerBarPosition = float(stretchableManager.getItemCurrentPosition(1)) / getWidth();
    return String(resizerBarPosition)+" "+String(leftComponentType)+" "+String(rightComponentType);
}

void Polytempo_ComposerMainView::restoreComponentStateFromString(const String& string)
{
    StringArray tokens;
    tokens.addTokens(string, false);
    tokens.removeEmptyStrings();
    tokens.trim();
    
    resizerBarPosition = tokens[0].getFloatValue();
    setLeftComponent((componentType)tokens[1].getIntValue());
    setRightComponent((componentType)tokens[2].getIntValue());
    
    resized();
}

void Polytempo_ComposerMainView::setLeftComponent(componentType type)
{
    if(type == leftComponentType)  leftComponentType = componentType_None;
    else                           leftComponentType = type;

    toolbarComponent->setComponentTypes(leftComponentType, rightComponentType);

    leftComponent.removeAllChildren();
    
    float position = float(stretchableManager.getItemCurrentPosition(1)) / getWidth();
    if(position > 0 && position < 1)
        resizerBarPosition = position; // to restore it later
    
    if(leftComponentType == componentType_None)
    {
        stretchableManager.setItemLayout(0,0,0,0);
        stretchableManager.setItemLayout(2,-1.0,-1.0,-1.0);
    }
    else if(leftComponentType != componentType_None && rightComponentType == componentType_None)
    {
        stretchableManager.setItemLayout(0,-1.0,-1.0,-1.0);
        stretchableManager.setItemLayout(2,0,0,0);
    }
    else
    {
        stretchableManager.setItemLayout(0,-0.1,-1.0,-resizerBarPosition);
        stretchableManager.setItemLayout(2,250,400,resizerBarPosition-1.0);
    }
    
    if(leftComponentType == componentType_TimeMap)       leftComponent.addAndMakeVisible(timeMapComponent.get());
    else if(leftComponentType == componentType_TempoMap) leftComponent.addAndMakeVisible(tempoMapComponent.get());

    resized();
}

Polytempo_ComposerMainView::componentType Polytempo_ComposerMainView::getLeftComponent()
{
    return leftComponentType;
}

void Polytempo_ComposerMainView::setRightComponent(componentType type)
{
    if(type == rightComponentType) rightComponentType = componentType_None;
    else                           rightComponentType = type;
    
    toolbarComponent->setComponentTypes(leftComponentType, rightComponentType);
    
    rightComponent.removeAllChildren();

    float position = float(stretchableManager.getItemCurrentPosition(1)) / getWidth();
    if(position > 0 && position < 1)
        resizerBarPosition = position; // to restore it later

    if(rightComponentType == componentType_None)
    {
        stretchableManager.setItemLayout(0,-1.0,-1.0,-1.0);
        stretchableManager.setItemLayout(2,0,0,0);
    }
    else if(rightComponentType != componentType_None && leftComponentType == componentType_None)
    {
        stretchableManager.setItemLayout(0,0,0,0);
        stretchableManager.setItemLayout(2,-1.0,-1.0,-1.0);
    }
    else
    {
        stretchableManager.setItemLayout(0,-0.1,-1.0,-resizerBarPosition);
        stretchableManager.setItemLayout(2,250,400,resizerBarPosition-1);
    }
    
    if(rightComponentType == componentType_PatternList)    rightComponent.addAndMakeVisible(beatPatternListComponent);
    else if(rightComponentType == componentType_PointList) rightComponent.addAndMakeVisible(pointListComponent);

    resized();
}
