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
    m.addCommandItem(commandManager, Polytempo_CommandIDs::removeControlPoints);
    m.addCommandItem(commandManager, Polytempo_CommandIDs::shiftControlPoints);
    m.addCommandItem(commandManager, Polytempo_CommandIDs::adjustControlPoints);
    m.addCommandItem(commandManager, Polytempo_CommandIDs::adjustTempo);

    m.show();
}

//-----------------------

Polytempo_CoordinateSystemComponent::Polytempo_CoordinateSystemComponent()
{
    playhead.reset(new DrawableRectangle());
    playhead->setFill(Colours::black);
    addAndMakeVisible(playhead.get());
}

Polytempo_CoordinateSystemComponent::~Polytempo_CoordinateSystemComponent()
{
    playhead = nullptr;
    draggedControlPointsOrigin.clear();
}

void Polytempo_CoordinateSystemComponent::setSizeAndZooms(int w, int h, float zX, float zY)
{
    int width, height;

    if(w > 0) width  = w; else width  = getWidth();
    if(h > 0) height = h; else height = getHeight();
    
    zoomX = zX;
    zoomY = zY;

    setSize(width, height);

    playhead->setSize(playhead->getWidth(), height);
}

void Polytempo_CoordinateSystemComponent::eventNotification(Polytempo_Event *event)
{
    if(event->getType() == eventType_Tick)
    {
        playhead->setRectangle(Rectangle<float> (TIMEMAP_OFFSET + (float)event->getValue() * zoomX, 0, 2.0f, (float) (getHeight())));
    }
}

void Polytempo_CoordinateSystemComponent::mouseUp(const MouseEvent &event)
{
    selectionRectangle.setWidth(0);
    draggedControlPointsOrigin.clearQuick(true);

    Polytempo_ComposerApplication::getCommandManager().commandStatusChanged(); // update menubar
    repaint();
}

//------------------------------------------------------------------------------
#pragma mark -
#pragma mark time map coordinate system component

Polytempo_TimeMapCoordinateSystem::Polytempo_TimeMapCoordinateSystem(Viewport *vp)
{
    viewport = vp;
    
    zoomX = float(Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("zoomX"));
    zoomY = float(Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("timeMapZoomY"));
}

void Polytempo_TimeMapCoordinateSystem::paint(Graphics& g)
{
    g.fillAll(Colour(255,255,255));
 
    Polytempo_Composition* composition = Polytempo_Composition::getInstance();
    Polytempo_Sequence* sequence = Polytempo_Composition::getInstance()->getSelectedSequence();
    if(sequence == nullptr) return;

    
    // vertical grid lines (time)
    
    int increment = Polytempo_TimeRuler::getIncrementForZoom(zoomX);
    g.setColour(Colour(50,50,50));
    for(int i=0;i<100;i++)
        g.drawVerticalLine(TIMEMAP_OFFSET + int(i * increment * zoomX), 0.0f, float(getHeight()));
    
    
    // horizontal grid lines (position)
    
    Polytempo_Event* event;
    float thickness, y;
    int i = -1;
    while((event = sequence->getEvent(++i)) != nullptr)
    {
        if(event->getType() == eventType_Beat)
        {
            y = getHeight() - TIMEMAP_OFFSET - event->getPosition() * zoomY;
            if(int(event->getProperty(eventPropertyString_Pattern)) == 10)
                thickness = 2.0;
            else if(int(event->getProperty(eventPropertyString_Pattern)) < 20)
                thickness = 1.0;
            else
                thickness = 0.5;
            
            g.drawLine(0, y, float(getWidth()), y, thickness);
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
    
    // draw the selection rectangle
    if(selectionRectangle.getWidth() > 0 && selectionRectangle.getHeight() > 0)
    {
        g.setColour(Colours::grey.withAlpha(0.15f));
        g.fillRect(selectionRectangle);
    }
}

void Polytempo_TimeMapCoordinateSystem::paintSequence(Graphics& g, Polytempo_Sequence* sequence, bool selected)
{
    Polytempo_Composition* composition = Polytempo_Composition::getInstance();
 
    Colour sequenceColour = sequence->getColour();
    float brightness = selected ? 1.0f : 0.5f;
    float alpha      = selected ? 1.0f : 0.5f;
    Polytempo_ControlPoint *controlPoint, *controlPoint1;
    int i = -1;
    while((controlPoint = sequence->getControlPoint(++i)) != nullptr)
    {
        int x = TIMEMAP_OFFSET + int(controlPoint->time * zoomX);
        int y = getHeight() - TIMEMAP_OFFSET - int(controlPoint->position * zoomY);

        
        // line segment between control points
        
        if((controlPoint1 = sequence->getControlPoint(i+1)) != nullptr &&
           !controlPoint1->start)
        {
            float x1, x2, y1, y2;
            float thickness = 2;
            int num = 20; // number of segments to draw
            
            if(Polytempo_TempoInterpolation::validateCurve(controlPoint, controlPoint1))
            {
                // the curve is valid
                Array < Point < float >* > points = Polytempo_TempoInterpolation::getPoints(num, controlPoint, controlPoint1);
                
                if(sequence->isTempoConstantAfterPoint(i))
                    thickness = 4;
                
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
            else
            {
                // the curve is invalid
                Array < Point < float >* > points = Polytempo_TempoInterpolation::getPoints(2, controlPoint, controlPoint1);
                g.setColour(sequenceColour.withMultipliedBrightness(brightness).withAlpha(alpha));
                thickness = 1;
                x1 = TIMEMAP_OFFSET + controlPoint->time * zoomX;
                y1 = getHeight() - TIMEMAP_OFFSET - controlPoint->position * zoomY;
                x2 = TIMEMAP_OFFSET + controlPoint1->time * zoomX;
                y2 = getHeight() - TIMEMAP_OFFSET - controlPoint1->position * zoomY;
                g.drawLine(x1,y1,x2,y1,thickness+1);
                g.drawLine(x2,y1,x2,y2,thickness);

            }
        }
        
        if(selected && controlPoint->isCoinciding)
        {
            g.setColour(Colours::black.withAlpha(0.25f));
            g.drawLine(float(x), 0.0f, float(x), float(getHeight()), 4.0f);
        }
        
        // control points

        if(selected &&
           composition->isSelectedControlPointIndex(i))
            g.setColour(sequenceColour);
        else
            g.setColour(Colour(0xffdddddd));
        
        float cornerFactor = controlPoint->start ? 0.1f : 0.5f;

        g.fillRoundedRectangle(x - CONTROL_POINT_SIZE * 0.5f,
                               y - CONTROL_POINT_SIZE * 0.5f,
                               CONTROL_POINT_SIZE,
                               CONTROL_POINT_SIZE,
                               CONTROL_POINT_SIZE * cornerFactor);
        
        if(!selected && controlPoint->isCoinciding)
            g.setColour(Colours::black);
        else
            g.setColour(sequenceColour.withMultipliedBrightness(brightness).withAlpha(alpha));
        
        float thickness = selected ? 2.0f : 1.0f;

        g.drawRoundedRectangle(x - CONTROL_POINT_SIZE * 0.5f,
                               y - CONTROL_POINT_SIZE * 0.5f,
                               CONTROL_POINT_SIZE,
                               CONTROL_POINT_SIZE,
                               CONTROL_POINT_SIZE * cornerFactor,
                               thickness);
        
        if(!controlPoint->cueIn.getPattern().isEmpty())
        {
            if(selected)
            {
                g.setColour(Colours::white);
                g.fillRect(x - CONTROL_POINT_SIZE * 2.0f, y - CONTROL_POINT_SIZE * 0.6f, CONTROL_POINT_SIZE * 1.3f, CONTROL_POINT_SIZE * 1.3f);
                g.setColour(sequenceColour.withMultipliedBrightness(brightness).withAlpha(alpha * 0.4f));
                g.fillRect(x - CONTROL_POINT_SIZE * 2.0f, y - CONTROL_POINT_SIZE * 0.6f, CONTROL_POINT_SIZE * 1.3f, CONTROL_POINT_SIZE * 1.3f);
                g.setColour(sequenceColour.withMultipliedBrightness(0.5f));
            }
            else
            {
                g.setColour(sequenceColour.withMultipliedBrightness(brightness).withAlpha(alpha));
            }
            g.drawSingleLineText("C", x - CONTROL_POINT_SIZE * 1.8f, y + CONTROL_POINT_SIZE * 0.5f);
        }
    }
}

void Polytempo_TimeMapCoordinateSystem::mouseDown(const MouseEvent &mouseEvent)
{
    float mouseTime           = (mouseEvent.x - TIMEMAP_OFFSET) / zoomX;
    float mouseFloatPos       = (getHeight() - mouseEvent.y - TIMEMAP_OFFSET) / zoomY;
    Rational mouseRationalPos = Rational(mouseFloatPos);

    Polytempo_Composition* composition = Polytempo_Composition::getInstance();
    Polytempo_Sequence* sequence = composition->getSelectedSequence();
    
    bool canDrag = false;

    // show pop-up menu
    if(mouseEvent.mods.isPopupMenu())
    {
        ((Polytempo_CoordinateSystem*)viewport)->showPopupMenu();
        return;
    }

    // cmd-click: add control point
    if(mouseEvent.mods.isCommandDown())
    {
        // quantize position
        OwnedArray<Polytempo_Event> *events = &sequence->getEvents();
        for(int i=0;i<events->size()-1;i++)
        {
            if(mouseFloatPos >= events->getUnchecked(i)->getPosition().toFloat() &&
               mouseFloatPos <  events->getUnchecked(i+1)->getPosition().toFloat())
            {
                if(fabs(mouseFloatPos - events->getUnchecked(i)->getPosition().toFloat()) <
                   fabs(mouseFloatPos -  events->getUnchecked(i+1)->getPosition().toFloat()))
                        mouseRationalPos = events->getUnchecked(i)->getPosition();
                else
                    mouseRationalPos = events->getUnchecked(i+1)->getPosition();
                break;
            }
        }
        if(sequence->validateNewControlPointPosition(mouseTime, mouseRationalPos))
        {
            sequence->addControlPoint(mouseTime, mouseRationalPos);
            composition->updateContent();
            canDrag = true;
            return;
        }
    }
    
    // hit detection
    hit = false;
    int i = -1;
    Polytempo_ControlPoint *controlPoint;
    while((controlPoint = sequence->getControlPoint(++i)) != nullptr)
    {
        // stop searching in this sequence if the point coordinates
        // are beyond the mouse coordinates
        if(controlPoint->time > mouseTime + CONTROL_POINT_SIZE / zoomX ||
           controlPoint->position.toFloat() > mouseFloatPos + CONTROL_POINT_SIZE / zoomY) break;

        if(fabs(controlPoint->time - mouseTime) <= CONTROL_POINT_SIZE * 0.5 / zoomX &&
           fabs(controlPoint->position.toFloat() - mouseFloatPos) <= CONTROL_POINT_SIZE * 0.5 / zoomY)
        {
            hit = true;
            if(mouseEvent.mods.isShiftDown())
            {
                if(composition->isSelectedControlPointIndex(i))
                {
                    composition->removeSelectedControlPointIndex(i);
                }
                else
                {
                    composition->addSelectedControlPointIndex(i);
                    canDrag = true;
                    draggedPoint = controlPoint;
                }
            }
            else
            {
                if(!composition->isSelectedControlPointIndex(i))
                {
                    
                    composition->clearSelectedControlPointIndices();
                    composition->addSelectedControlPointIndex(i);
                }
                canDrag = true;
                draggedPoint = controlPoint;
            }
            break;
        }
    }
    // no hit detected
    if(!hit && !mouseEvent.mods.isShiftDown()) composition->clearSelectedControlPointIndices();
    
    // prepare for dragging
    if(canDrag)
    {
        for(int index : *composition->getSelectedControlPointIndices())
        {
            Polytempo_ControlPoint *temp = sequence->getControlPoint(index)->copy();
            draggedControlPointsOrigin.add(temp);
            if(draggedPoint == sequence->getControlPoint(index)) draggedPoint = temp;
        }
    }

    composition->updateContent(); // repaint everything
}

void Polytempo_TimeMapCoordinateSystem::mouseDrag(const MouseEvent &mouseEvent)
{
    if(mouseEvent.mods.isPopupMenu()) return;
    if(mouseEvent.getDistanceFromDragStart() < 1) return;
        
    Polytempo_Composition* composition = Polytempo_Composition::getInstance();
    Polytempo_Sequence* sequence = composition->getSelectedSequence();

    if(draggedControlPointsOrigin.size() > 0)
    {
        float deltaT;
        Rational deltaPos;

        float mouseTime           = (mouseEvent.x - TIMEMAP_OFFSET) / zoomX;
        float mouseFloatPos       = (getHeight() - mouseEvent.y - TIMEMAP_OFFSET) / zoomY;
        Rational mouseRationalPos = Rational(mouseFloatPos);

        // quantize time
        mouseTime = int(mouseTime * 10) * 0.1f;

        // quantize position
        OwnedArray<Polytempo_Event> *events = &sequence->getEvents();
        if(mouseRationalPos > events->getLast()->getPosition()) // mouse is outside of beat pattern structure
        {
            mouseRationalPos = Rational(1,4) * int((mouseRationalPos * Rational(4)).toFloat());
            // position is quantized to multiples of 1/4. (Is 1/4 the best solution?)
        }
        else
        {
            for(int i=0;i<events->size()-1;i++)
            {
                if(mouseFloatPos >= events->getUnchecked(i)->getPosition().toFloat() &&
                   mouseFloatPos <  events->getUnchecked(i+1)->getPosition().toFloat())
                {
                    if(fabs(mouseFloatPos - events->getUnchecked(i)->getPosition().toFloat()) <
                       fabs(mouseFloatPos -  events->getUnchecked(i+1)->getPosition().toFloat()))
                            mouseRationalPos = events->getUnchecked(i)->getPosition();
                    else
                        mouseRationalPos = events->getUnchecked(i+1)->getPosition();
                    break;
                }
            }
        }
        
        // calculate delta
        deltaT = mouseTime - draggedPoint->time ;
        deltaPos = mouseEvent.mods.isShiftDown() ? 0 : mouseRationalPos - draggedPoint->position;

        for(int i=0;i<draggedControlPointsOrigin.size();i++)
        {
            sequence->setControlPointTime(composition->getSelectedControlPointIndices()->getUnchecked(i),
                                          draggedControlPointsOrigin.getUnchecked(i)->time + deltaT);
            sequence->setControlPointPosition(composition->getSelectedControlPointIndices()->getUnchecked(i),
                                              draggedControlPointsOrigin.getUnchecked(i)->position + deltaPos);
        }
        sequence->update();
        
        composition->updateContent(); // repaint everything
    }
    else
    {
        selectionRectangle.setLeft  (mouseEvent.position.x < mouseEvent.mouseDownPosition.x ? mouseEvent.position.x : mouseEvent.mouseDownPosition.x);
        selectionRectangle.setTop   (mouseEvent.position.y < mouseEvent.mouseDownPosition.y ? mouseEvent.position.y : mouseEvent.mouseDownPosition.y);
        selectionRectangle.setRight (mouseEvent.position.x > mouseEvent.mouseDownPosition.x ? mouseEvent.position.x : mouseEvent.mouseDownPosition.x);
        selectionRectangle.setBottom(mouseEvent.position.y > mouseEvent.mouseDownPosition.y ? mouseEvent.position.y : mouseEvent.mouseDownPosition.y);
        
        composition->clearSelectedControlPointIndices();
        int i = -1;
        float x, y;
        Polytempo_ControlPoint *controlPoint;
        while((controlPoint = sequence->getControlPoint(++i)) != nullptr)
        {
            x = TIMEMAP_OFFSET + controlPoint->time * zoomX;
            y = getHeight() - TIMEMAP_OFFSET - controlPoint->position * zoomY;

            // stop searching in this sequence if the point coordinates
            // are beyond the mouse coordinates
            if(x > selectionRectangle.getRight() || y < selectionRectangle.getY()) break;

            if(selectionRectangle.contains(x,y)) composition->addSelectedControlPointIndex(i);
        }

        composition->updateContent(); // repaint everything (also list view)
    }
}

//------------------------------------------------------------------------------
#pragma mark -
#pragma mark tempo map coordinate system component

Polytempo_TempoMapCoordinateSystem::Polytempo_TempoMapCoordinateSystem(Viewport *vp)
{
    viewport = vp;

    zoomX = float(Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("zoomX"));
    zoomY = float(Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("tempoMapZoomY"));
}

void Polytempo_TempoMapCoordinateSystem::paint(Graphics& g)
{
    g.fillAll(Colour(255,255,255));
    
    Polytempo_Composition* composition = Polytempo_Composition::getInstance();
    Polytempo_Sequence* sequence = Polytempo_Composition::getInstance()->getSelectedSequence();
    if(sequence == nullptr) return;
    
    
    // vertical grid lines (time)
    
    int timeIncrement = Polytempo_TimeRuler::getIncrementForZoom(zoomX);
    g.setColour(Colour(50,50,50));
    for(int i=0;i<100;i++)
        g.drawVerticalLine(TIMEMAP_OFFSET + int(i * timeIncrement * zoomX), 0.0f, float(getHeight()));
    
    
    // horizontal grid lines (tempo)

    float tempoIncrement = Polytempo_TempoRuler::getIncrementForZoom(zoomY);
    float tempo = 0;
    
    while(tempo < 5.0f)
    {
        float y = getHeight() - TIMEMAP_OFFSET - tempo * zoomY;
        g.drawLine(0, y, float(getWidth()), y, 0.5f);
        tempo += tempoIncrement;
    }
    
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

    // draw the selection rectangle
    if(selectionRectangle.getWidth() > 0 && selectionRectangle.getHeight() > 0)
    {
        g.setColour(Colours::grey.withAlpha(0.15f));
        g.fillRect(selectionRectangle);
    }
}

void Polytempo_TempoMapCoordinateSystem::paintSequence(Graphics& g, Polytempo_Sequence* sequence, bool selected)
{
    Polytempo_Composition* composition = Polytempo_Composition::getInstance();
    
    Colour sequenceColour = sequence->getColour();
    float brightness = selected ? 1.0f : 0.5f;
    float alpha      = selected ? 1.0f : 0.5f;
    Polytempo_ControlPoint *controlPoint, *controlPoint1;
    int i = -1;
    while((controlPoint = sequence->getControlPoint(++i)) != nullptr)
    {
        int x = TIMEMAP_OFFSET + int(controlPoint->time * zoomX);
        int y = controlPoint->start ? getHeight() - TIMEMAP_OFFSET - int(controlPoint->tempoOut * zoomY)
                      :  getHeight() - TIMEMAP_OFFSET - int(controlPoint->tempoIn * zoomY);
        
        
        // line segment between control points
        
        if((controlPoint1 = sequence->getControlPoint(i+1)) != nullptr &&
           !controlPoint1->start)
        {
            float x1, x2, y1, y2;
            float thickness = 1;
            int num = 20; // number of segments to draw
            
            if(Polytempo_TempoInterpolation::validateCurve(controlPoint, controlPoint1))
            {
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
        }
        
        if(selected && controlPoint->isCoinciding)
        {
            g.setColour(Colours::black.withAlpha(0.25f));
            g.drawLine(float(x), 0.0f, float(x), float(getHeight()), 4.0f);
        }
        
        // control points
        
        int y1 = y;
        if((controlPoint1 = sequence->getControlPoint(i+1)) != nullptr &&
           !controlPoint1->start)
            y1 = getHeight() - TIMEMAP_OFFSET - int(controlPoint->tempoOut * zoomY);
        int height = abs(y-y1);
        y = y < y1 ? y : y1;
        
        if(selected &&
           composition->isSelectedControlPointIndex(i))
            g.setColour(sequenceColour);
        else
            g.setColour(Colour(0xffdddddd));
        
        float cornerFactor = controlPoint->start ? 0.1f : 0.5f;

        g.fillRoundedRectangle(x - CONTROL_POINT_SIZE * 0.5f,
                               y - CONTROL_POINT_SIZE * 0.5f,
                               CONTROL_POINT_SIZE,
                               CONTROL_POINT_SIZE + float(height),
                               CONTROL_POINT_SIZE * cornerFactor);
        
        if(!selected && controlPoint->isCoinciding)
            g.setColour(Colours::black);
        else
            g.setColour(sequenceColour.withMultipliedBrightness(brightness).withAlpha(alpha));
        
        float thickness = selected ? 2.0f : 1.0f;
        g.drawRoundedRectangle(x - CONTROL_POINT_SIZE * 0.5f,
                               y - CONTROL_POINT_SIZE * 0.5f,
                               CONTROL_POINT_SIZE,
                               CONTROL_POINT_SIZE + float(height),
                               CONTROL_POINT_SIZE * cornerFactor,
                               thickness);
        
        if(!controlPoint->cueIn.getPattern().isEmpty())
        {
            y = getHeight() - TIMEMAP_OFFSET - int(controlPoint->tempoOut * zoomY);
            if(selected)
            {
                g.setColour(Colours::white);
                g.fillRect(x - CONTROL_POINT_SIZE * 2.0f, y - CONTROL_POINT_SIZE * 0.6f, CONTROL_POINT_SIZE * 1.3f, CONTROL_POINT_SIZE * 1.3f);
                g.setColour(sequenceColour.withMultipliedBrightness(brightness).withAlpha(alpha * 0.4f));
                g.fillRect(x - CONTROL_POINT_SIZE * 2.0f, y - CONTROL_POINT_SIZE * 0.6f, CONTROL_POINT_SIZE * 1.3f, CONTROL_POINT_SIZE * 1.3f);
                g.setColour(sequenceColour.withMultipliedBrightness(0.5f));
            }
            else
            {
                g.setColour(sequenceColour.withMultipliedBrightness(brightness).withAlpha(alpha));
            }
            g.drawSingleLineText("C", x - CONTROL_POINT_SIZE * 1.8f, y + CONTROL_POINT_SIZE * 0.5f);
        }
    }
}

void Polytempo_TempoMapCoordinateSystem::mouseDown(const MouseEvent &event)
{
    float mouseTime      = (event.x - TIMEMAP_OFFSET) / zoomX;
    float mousePosition  = (getHeight() - event.y - TIMEMAP_OFFSET) / zoomY;

    Polytempo_Composition* composition = Polytempo_Composition::getInstance();
    Polytempo_Sequence* sequence = composition->getSelectedSequence();
    
    // show pop-up menu
    if(event.mods.isPopupMenu())
    {
       ((Polytempo_CoordinateSystem*)viewport)->showPopupMenu();
        return;
    }

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
    
    // hit detection
    hit = false;
    int i = -1;
    Polytempo_ControlPoint *controlPoint;
    while((controlPoint = sequence->getControlPoint(++i)) != nullptr)
    {
        // stop searching in this sequence if the point coordinates
        // are beyond the mouse coordinates
        if(controlPoint->time > mouseTime + CONTROL_POINT_SIZE / zoomX) break;

        if(fabs(controlPoint->time - mouseTime) <= CONTROL_POINT_SIZE * 0.5 / zoomX &&
           (fabs(controlPoint->tempoIn - mousePosition) <= CONTROL_POINT_SIZE * 0.5 / zoomY ||
            fabs(controlPoint->tempoOut - mousePosition) <= CONTROL_POINT_SIZE * 0.5 / zoomY ||
            controlPoint->tempoIn - controlPoint->tempoOut) >= mousePosition - controlPoint->tempoOut)
        {
            hit = true;
            if(event.mods.isShiftDown())
            {
                if(composition->isSelectedControlPointIndex(i)) composition->removeSelectedControlPointIndex(i);
                else                                            composition->addSelectedControlPointIndex(i);
            }
            else
            {
                if(!composition->isSelectedControlPointIndex(i))
                {
                    composition->clearSelectedControlPointIndices();
                    composition->addSelectedControlPointIndex(i);
                }
            }
            break;
        }
    }
    // no hit detected
    if(!hit && !event.mods.isShiftDown()) composition->clearSelectedControlPointIndices();

    composition->updateContent(); // repaint everything
}

void Polytempo_TempoMapCoordinateSystem::mouseDrag(const MouseEvent &event)
{
    if(!hit)
    {
        Polytempo_Composition* composition = Polytempo_Composition::getInstance();
        Polytempo_Sequence* sequence = composition->getSelectedSequence();

        selectionRectangle.setLeft  (event.position.x < event.mouseDownPosition.x ? event.position.x : event.mouseDownPosition.x);
        selectionRectangle.setTop   (event.position.y < event.mouseDownPosition.y ? event.position.y : event.mouseDownPosition.y);
        selectionRectangle.setRight (event.position.x > event.mouseDownPosition.x ? event.position.x : event.mouseDownPosition.x);
        selectionRectangle.setBottom(event.position.y > event.mouseDownPosition.y ? event.position.y : event.mouseDownPosition.y);

        composition->clearSelectedControlPointIndices();
        int i = -1;
        float x, y, y1, y2;
        Polytempo_ControlPoint *controlPoint;
        while((controlPoint = sequence->getControlPoint(++i)) != nullptr)
        {
            x = TIMEMAP_OFFSET + controlPoint->time * zoomX;
            y1 = getHeight() - TIMEMAP_OFFSET - (controlPoint->tempoIn * zoomY);
            y2 = getHeight() - TIMEMAP_OFFSET - (controlPoint->tempoOut * zoomY);
            if(y1 > y2)
            {
                y = y1; y1 = y2; y2 = y;
            }
            
            // stop searching in this sequence if the point coordinates
            // are beyond the mouse coordinates
            if(x > selectionRectangle.getRight() && y1 < selectionRectangle.getY()) break;

            if(selectionRectangle.intersects(Line<float>(x, y1, x, y2)))
                composition->addSelectedControlPointIndex(i);
        }

        composition->updateContent(); // repaint everything (also list view)
    }
}
