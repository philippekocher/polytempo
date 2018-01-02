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

#include "Polytempo_CoordinateSystem.h"
#include "../../Misc/Polytempo_Globals.h"
#include "../../Preferences/Polytempo_StoredPreferences.h"
#include "Polytempo_Ruler.h"
#include "../../Application/PolytempoComposer/Polytempo_ComposerApplication.h"
#include "../../Application/Polytempo_CommandIDs.h"


Polytempo_CoordinateSystem::Polytempo_CoordinateSystem()
{
    setScrollBarsShown(true, false, true, true);
}

Polytempo_CoordinateSystem::~Polytempo_CoordinateSystem()
{}

void Polytempo_CoordinateSystem::visibleAreaChanged(const Rectangle<int> &r)
{
    if(synchronizedViewport[0] != nullptr)
         synchronizedViewport[0]->setViewPosition(r.getX(),synchronizedViewport[0]->getViewPositionY());

    if(synchronizedViewport[1] != nullptr)
            synchronizedViewport[1]->setViewPosition(synchronizedViewport[1]->getViewPositionX(),r.getY());
}

void Polytempo_CoordinateSystem::setSynchronizedViewport(Viewport *sync, int axis)
{
    synchronizedViewport[axis] = sync;
}

void Polytempo_CoordinateSystem::showPopupMenu()
{
    ApplicationCommandManager* commandManager = &Polytempo_ComposerApplication::getCommandManager();
    PopupMenu m;
    
    m.addCommandItem(commandManager, Polytempo_CommandIDs::insertControlPoint);
    m.addCommandItem(commandManager, Polytempo_CommandIDs::removeControlPoint);

    m.addSeparator();
    
    m.addCommandItem(commandManager, Polytempo_CommandIDs::adjustTime);
    m.addCommandItem(commandManager, Polytempo_CommandIDs::adjustPosition);
    m.addCommandItem(commandManager, Polytempo_CommandIDs::adjustTempo);
 
    m.show();
}

//-----------------------

Polytempo_CoordinateSystemComponent::Polytempo_CoordinateSystemComponent()
{
    playhead = new DrawableRectangle();
    playhead->setFill(Colours::black);
    addAndMakeVisible(playhead);

    positionGrid = new Array < float >();
}

Polytempo_CoordinateSystemComponent::~Polytempo_CoordinateSystemComponent()
{
    playhead = nullptr;
}

void Polytempo_CoordinateSystemComponent::eventNotification(Polytempo_Event *event)
{
    if(event->getType() == eventType_Tick)
    {
        playhead->setRectangle(Rectangle<float> (TIMEMAP_OFFSET + (float)event->getValue() * zoomX, 0, 2.0f, (float) (getHeight())));
    }
}

//------------------------------------------------------------------------------
#pragma mark -
#pragma mark time map coordinate system component

Polytempo_TimeMapCoordinateSystem::Polytempo_TimeMapCoordinateSystem(Viewport *vp)
{
    viewport = vp;
    
    zoomX = Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("zoomX");
    zoomY = Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("timeMapZoomY");

    Polytempo_StoredPreferences::getInstance()->getProps().addChangeListener(this);
}

void Polytempo_TimeMapCoordinateSystem::changeListenerCallback(ChangeBroadcaster* bc)
{
    // scroll friendly zoom
    
    float x = (viewport->getViewPositionX() - TIMEMAP_OFFSET) / zoomX;
    float y = (getHeight() - viewport->getViewPositionY() - TIMEMAP_OFFSET - viewport->getMaximumVisibleHeight()) / zoomY;
    
    zoomX = Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("zoomX");
    zoomY = Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("timeMapZoomY");
    
    repaint();
    
    viewport->setViewPosition(TIMEMAP_OFFSET + x*zoomX, getHeight() - y*zoomY - TIMEMAP_OFFSET - viewport->getMaximumVisibleHeight());
}

void Polytempo_TimeMapCoordinateSystem::paint(Graphics& g)
{
    g.fillAll(Colour(255,255,255));
 
    Polytempo_Composition* composition = Polytempo_Composition::getInstance();
    Polytempo_Sequence* sequence;
    if(!(sequence = Polytempo_Composition::getInstance()->getSelectedSequence())) return;

    
    // vertical grid lines (time)
    
    int increment = Polytempo_TimeRuler::getIncrementForZoom(zoomX);
    g.setColour(Colour(50,50,50));
    for(int i=0;i<100;i++)
        g.drawVerticalLine(TIMEMAP_OFFSET + i * increment * zoomX, 0, getHeight());
    
    
    // horizontal grid lines (position)
    
    positionGrid->clearQuick();
    
    Polytempo_Event* event;
    Rational pos;
    float thickness, y;
    int i = -1;
    while((event = sequence->getEvent(++i)) != nullptr)
    {
        pos = event->getPosition();
        if(event->getType() == eventType_Beat)
        {
            y = getHeight() - TIMEMAP_OFFSET - pos * zoomY;
            positionGrid->add(pos.toFloat());
            if(int(event->getProperty(eventPropertyString_Pattern)) == 10)
                thickness = 2.0;
            else if(int(event->getProperty(eventPropertyString_Pattern)) < 20)
                thickness = 1.0;
            else
                thickness = 0.5;
            
            g.drawLine(0, y, getWidth(), y, thickness);
        }
    }
    
    // control points and curve
    
    i = -1;
    while((sequence = composition->getSequence(++i)) != nullptr)
    {
        if(sequence->isVisible() && sequence != composition->getSelectedSequence())
        {
            paintSequence(g, sequence, false);
        }
    }
    
    // draw the selected sequence last
    paintSequence(g, composition->getSelectedSequence(), true);
}

void Polytempo_TimeMapCoordinateSystem::paintSequence(Graphics& g, Polytempo_Sequence* sequence, bool selected)
{
    Polytempo_Composition* composition = Polytempo_Composition::getInstance();
 
    Colour sequenceColour = sequence->getColour();
    float brightness = selected ? 1.0 : 0.5;
    float alpha      = selected ? 1.0 : 0.5;
    Polytempo_ControlPoint *controlPoint, *controlPoint1;
    int i = -1;
    while((controlPoint = sequence->getControlPoint(++i)) != nullptr)
    {
        int x = TIMEMAP_OFFSET + controlPoint->time * zoomX;
        int y = getHeight() - TIMEMAP_OFFSET - controlPoint->position * zoomY;

        
        // line segment between control points
        
        if((controlPoint1 = sequence->getControlPoint(i+1)) != nullptr)
        {
            float x1, x2, y1, y2;
            float thickness = 1;
            int num = 20; // number of segments to draw
            
            Array < Point < float >* > points = Polytempo_TempoInterpolation::getPoints(num, controlPoint, controlPoint1);
            
            if(sequence->isTempoConstantAfterPoint(i))
                thickness = 3;
            
            g.setColour(sequenceColour.withMultipliedBrightness(brightness).withAlpha(alpha));
            
            for(int n=0;n<points.size()-1;n++)
            {
                x1 = TIMEMAP_OFFSET + points[n]->x * zoomX;
                y1 = getHeight() - TIMEMAP_OFFSET -points[n]->y * zoomY;
                x2 = TIMEMAP_OFFSET + points[n+1]->x * zoomX;
                y2 = getHeight() - TIMEMAP_OFFSET -points[n+1]->y * zoomY;
                
                g.drawLine(x1,y1,x2,y2,thickness);
            }
        }
        
        if(selected && controlPoint->isCoinciding)
        {
            g.setColour(Colours::black.withAlpha(0.25f));
            g.drawLine(x, 0, x, getHeight(), 4.0);
        }
        
        // control points

        if(selected &&
           composition->getSelectedControlPointIndex() == i)
            g.setColour(sequenceColour);
        else
            g.setColour(Colour(0xffdddddd));
        
        g.fillEllipse(x - CONTROL_POINT_SIZE * 0.5,
                      y - CONTROL_POINT_SIZE * 0.5,
                      CONTROL_POINT_SIZE,
                      CONTROL_POINT_SIZE);
        
        if(!selected && controlPoint->isCoinciding)
            g.setColour(Colours::black);
        else
            g.setColour(sequenceColour.withMultipliedBrightness(brightness).withAlpha(alpha));
        
        float thickness = selected ? 2.0 : 1.0;
        g.drawEllipse(x - CONTROL_POINT_SIZE * 0.5,
                      y - CONTROL_POINT_SIZE * 0.5,
                      CONTROL_POINT_SIZE,
                      CONTROL_POINT_SIZE,
                      thickness);
    }
}


bool draggingSession = false;

void Polytempo_TimeMapCoordinateSystem::mouseDown(const MouseEvent &event)
{
    float mouseTime      = (event.x - TIMEMAP_OFFSET) / zoomX;
    float mousePosition  = (getHeight() - event.y - TIMEMAP_OFFSET) / zoomY;

    Polytempo_Composition* composition = Polytempo_Composition::getInstance();
    Polytempo_Sequence* sequence = composition->getSelectedSequence();
    
    
    // cmd-click: add control point
    if(event.mods.isCommandDown())
    {
        if(sequence->validateNewControlPointPosition(mouseTime, mousePosition))
        {
            sequence->addControlPoint(mouseTime, mousePosition);
            composition->updateContent();
            return;
        }
    }
    
    draggingSession = false;
    
    // hit test
    int hit = -1, i = -1;
    Polytempo_ControlPoint *controlPoint;
    while((controlPoint = sequence->getControlPoint(++i)) != nullptr)
    {
        // stop searching in this sequence if the point coordinates
        // are beyond the mouse coordinates
        if(controlPoint->time > mouseTime + CONTROL_POINT_SIZE / zoomX ||
           controlPoint->position.toFloat() > mousePosition + CONTROL_POINT_SIZE / zoomY) break;

        if(fabs(controlPoint->time - mouseTime) <= CONTROL_POINT_SIZE * 0.5 / zoomX &&
           fabs(controlPoint->position.toFloat() - mousePosition) <= CONTROL_POINT_SIZE * 0.5 / zoomY)
        {
            hit = i;
            break;
        }
    }

    if(event.mods.isPopupMenu())
    {
        if(hit != -1)
        {
            composition->setSelectedControlPointIndex(hit);
            composition->updateContent(); // repaint to show selection
            ((Polytempo_CoordinateSystem*)viewport)->showPopupMenu();
        }
        else
            ((Polytempo_CoordinateSystem*)viewport)->showPopupMenu();
    }
    else
    {
        composition->setSelectedControlPointIndex(hit);
    }
    composition->updateContent(); // repaint everything
}


void Polytempo_TimeMapCoordinateSystem::mouseDrag(const MouseEvent &event)
{
    if(event.mods.isPopupMenu()) return;
    if(event.getDistanceFromDragStart() > 1) draggingSession = true;
    
    if(draggingSession)
    {
        Polytempo_Composition* composition = Polytempo_Composition::getInstance();

        if(-1 == composition->getSelectedControlPointIndex()) return;
        
        float time, scorePosition;
        
        time = (event.x - TIMEMAP_OFFSET) / zoomX;
        scorePosition = event.mods.isShiftDown() ? -1 : (getHeight() - event.y - TIMEMAP_OFFSET) / zoomY;
        
        if(!event.mods.isCommandDown())
        {
            // quantize time
            time = int(time * 10) * 0.1;
 
            // quantize score position
            for(int i=0;i<positionGrid->size()-1;i++)
            {
                if(positionGrid->getUnchecked(i)   < scorePosition &&
                   positionGrid->getUnchecked(i+1) > scorePosition)
                {
                    if(fabs(positionGrid->getUnchecked(i) - scorePosition) <
                       fabs(positionGrid->getUnchecked(i+1) - scorePosition))
                        scorePosition = positionGrid->getUnchecked(i);
                    else
                        scorePosition = positionGrid->getUnchecked(i+1);
                    break;
                }
            }
        }
        
        composition->getSelectedSequence()->setControlPointPosition(composition->getSelectedControlPointIndex(), time, scorePosition);

        composition->updateContent(); // repaint everything
    }
}

//------------------------------------------------------------------------------
#pragma mark -
#pragma mark tempo map coordinate system component

Polytempo_TempoMapCoordinateSystem::Polytempo_TempoMapCoordinateSystem(Viewport *vp)
{
    viewport = vp;

    zoomX = Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("zoomX");
    zoomY = Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("tempoMapZoomY");

    Polytempo_StoredPreferences::getInstance()->getProps().addChangeListener(this);
}

void Polytempo_TempoMapCoordinateSystem::changeListenerCallback(ChangeBroadcaster* bc)
{
    // scroll friendly zoom
    
    float x = (viewport->getViewPositionX() - TIMEMAP_OFFSET) / zoomX;
    float y = (getHeight() - viewport->getViewPositionY() - TIMEMAP_OFFSET - viewport->getMaximumVisibleHeight()) / zoomY;
    
    zoomX = Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("zoomX");
    zoomY = Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("tempoMapZoomY");
    
    repaint();
    
    viewport->setViewPosition(TIMEMAP_OFFSET + x*zoomX, getHeight() - y*zoomY - TIMEMAP_OFFSET - viewport->getMaximumVisibleHeight());
}

void Polytempo_TempoMapCoordinateSystem::paint(Graphics& g)
{
    g.fillAll(Colour(255,255,255));
    
    Polytempo_Composition* composition = Polytempo_Composition::getInstance();
    Polytempo_Sequence* sequence;
    if(!(sequence = Polytempo_Composition::getInstance()->getSelectedSequence())) return;
    
    
    // vertical grid lines (time)
    
    int increment = Polytempo_TimeRuler::getIncrementForZoom(zoomX);
    g.setColour(Colour(50,50,50));
    for(int i=0;i<100;i++)
        g.drawVerticalLine(TIMEMAP_OFFSET + i * increment * zoomX, 0, getHeight());
    
    
    // horizontal grid lines (tempo)
    
    // cf. tempo ruler
    
    
    // control points and curve
    
    int i = -1;
    while((sequence = composition->getSequence(++i)) != nullptr)
    {
        if(sequence->isVisible() && sequence != composition->getSelectedSequence())
        {
            paintSequence(g, sequence, false);
        }
    }
    
    // draw the selected sequence last
    paintSequence(g, composition->getSelectedSequence(), true);
}

void Polytempo_TempoMapCoordinateSystem::paintSequence(Graphics& g, Polytempo_Sequence* sequence, bool selected)
{
    Polytempo_Composition* composition = Polytempo_Composition::getInstance();
    
    Colour sequenceColour = sequence->getColour();
    float brightness = selected ? 1.0 : 0.5;
    float alpha      = selected ? 1.0 : 0.5;
    Polytempo_ControlPoint *controlPoint, *controlPoint1;
    int i = -1;
    while((controlPoint = sequence->getControlPoint(++i)) != nullptr)
    {
        int x = TIMEMAP_OFFSET + controlPoint->time * zoomX;
        int y = i == 0 ? getHeight() - TIMEMAP_OFFSET - controlPoint->tempoOut * zoomY
                      :  getHeight() - TIMEMAP_OFFSET - controlPoint->tempoIn * zoomY;
        
        
        // line segment between control points
        
        if((controlPoint1 = sequence->getControlPoint(i+1)) != nullptr)
        {
            float x1, x2, y1, y2;
            float thickness = 1;
            int num = 20; // number of segments to draw
            
            Array < float > tempos = Polytempo_TempoInterpolation::getTempos(num, controlPoint, controlPoint1);
            Array < Point < float >* > points = Polytempo_TempoInterpolation::getPoints(num, controlPoint, controlPoint1);
            
            if(sequence->isTempoConstantAfterPoint(i))
                thickness = 3;
            
            g.setColour(sequenceColour.withMultipliedBrightness(brightness).withAlpha(alpha));
            
            for(int n=0;n<points.size()-1;n++)
            {
                x1 = TIMEMAP_OFFSET + points[n]->x * zoomX;
                y1 = getHeight() - TIMEMAP_OFFSET - tempos[n] * zoomY;
                x2 = TIMEMAP_OFFSET + points[n+1]->x * zoomX;
                y2 = getHeight() - TIMEMAP_OFFSET - tempos[n+1] * zoomY;
                
                g.drawLine(x1,y1,x2,y2,thickness);
            }
        }
        
        if(selected && controlPoint->isCoinciding)
        {
            g.setColour(Colours::black.withAlpha(0.25f));
            g.drawLine(x, 0, x, getHeight(), 4.0);
        }
        
        // control points
        
        int y1 = i < sequence->getControlPoints()->size() - 1 ?
                getHeight() - TIMEMAP_OFFSET - controlPoint->tempoOut * zoomY : y;
        int height = abs(y-y1);
        y = y < y1 ? y : y1;
        
        if(selected &&
           composition->getSelectedControlPointIndex() == i)
            g.setColour(sequenceColour);
        else
            g.setColour(Colour(0xffdddddd));
        
        g.fillRoundedRectangle(x - CONTROL_POINT_SIZE * 0.5,
                               y - CONTROL_POINT_SIZE * 0.5,
                               CONTROL_POINT_SIZE,
                               CONTROL_POINT_SIZE + height,
                               CONTROL_POINT_SIZE * 0.5);
        
        if(!selected && controlPoint->isCoinciding)
            g.setColour(Colours::black);
        else
            g.setColour(sequenceColour.withMultipliedBrightness(brightness).withAlpha(alpha));
        
        float thickness = selected ? 2.0 : 1.0;
        g.drawRoundedRectangle(x - CONTROL_POINT_SIZE * 0.5,
                               y - CONTROL_POINT_SIZE * 0.5,
                               CONTROL_POINT_SIZE,
                               CONTROL_POINT_SIZE + height,
                               CONTROL_POINT_SIZE * 0.5,
                               thickness);
    }
}
