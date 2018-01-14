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

#include "Polytempo_Ruler.h"
#include "../../Misc/Polytempo_Globals.h"
#include "../../Preferences/Polytempo_StoredPreferences.h"
#include "../../Data/Polytempo_Composition.h"
#include "../../Misc/Polytempo_TempoMeasurement.h"


Polytempo_Ruler::Polytempo_Ruler()
{}

Polytempo_Ruler::~Polytempo_Ruler()
{}

void Polytempo_Ruler::visibleAreaChanged(const Rectangle<int> &r)
{
    if(synchronizedViewport != nullptr)
    {
        if(synchronizedAxis == 0)
            synchronizedViewport->setViewPosition(r.getX(),synchronizedViewport->getViewPositionY());
        else if(synchronizedAxis == 1)
            synchronizedViewport->setViewPosition(synchronizedViewport->getViewPositionX(),r.getY());
        else
            synchronizedViewport->setViewPosition(r.getX(),r.getY());
    }
}

void Polytempo_Ruler::setSynchronizedViewport(Viewport *sync, int axis)
{
    synchronizedViewport = sync;
    synchronizedAxis = axis;
}

//------------------------------------------------------------------------------
#pragma mark -
#pragma mark time ruler

Polytempo_TimeRuler::Polytempo_TimeRuler()
{
    setScrollBarsShown(false, true);
    setViewedComponent(&rulerComponent, false);
    rulerComponent.setBounds(Rectangle<int> (2800, 40));
}

int Polytempo_TimeRuler::getIncrementForZoom(float zoomX)
{
    if(zoomX < 0.4)
    {
        return 300;
    }
    else if(zoomX < 0.8)
    {
        return 120;
    }
    else if(zoomX < 1.6)
    {
        return 60;
    }
    else if(zoomX < 3)
    {
        return 30;
    }
    else if(zoomX < 5)
    {
        return 20;
    }
    else if(zoomX < 10)
    {
        return 10;
    }
    else if(zoomX < 50)
    {
        return 5;
    }
    else
    {
        return 1;
    }
}

Polytempo_TimeRulerComponent::Polytempo_TimeRulerComponent()
{
    Polytempo_StoredPreferences::getInstance()->getProps().addChangeListener(this);
    zoomX = Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("zoomX");
}

void Polytempo_TimeRulerComponent::changeListenerCallback(ChangeBroadcaster *)
{
    zoomX = Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("zoomX");
    
    repaint();
}


void Polytempo_TimeRulerComponent::paint(Graphics& g)
{
    int hour, min, sec = 0;
    int increment = Polytempo_TimeRuler::getIncrementForZoom(zoomX);
       
    g.setColour(Colour(50,50,50));
    g.setFont(12);
    
    String label = String();
    for(int i=0;i<100;i++)
    {
        min = (sec / 60) % 60;
        hour = (sec / 3600);
        
        if(hour == 0)
        {
            label = "";
            label << String(min).paddedLeft('0',2) << ":" << String(sec%60).paddedLeft('0',2);
        }
        else
        {
            label = "";
            label << String(hour) << ":" << String(min).paddedLeft('0',2) << ":" << String(sec%60).paddedLeft('0',2);
        }
        g.drawVerticalLine(TIMEMAP_OFFSET + sec * zoomX, 0, getHeight());
        g.drawText(label, TIMEMAP_OFFSET + 2 + sec * zoomX, 1, 50, 12, Justification::left);

        sec += increment;
    }
}

//------------------------------------------------------------------------------
#pragma mark -
#pragma mark position ruler

Polytempo_PositionRuler::Polytempo_PositionRuler()
{
    setScrollBarsShown(false, false, true, true);
    setViewedComponent(&rulerComponent, false);
    rulerComponent.setBounds(Rectangle<int>(65, 2800));
    //setViewPositionProportionately (0.0, 1.0);
}

Polytempo_PositionRulerComponent::Polytempo_PositionRulerComponent()
{
    Polytempo_StoredPreferences::getInstance()->getProps().addChangeListener(this);
    zoomY = Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("timeMapZoomY");
}

void Polytempo_PositionRulerComponent::changeListenerCallback(ChangeBroadcaster *)
{
    zoomY = Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("timeMapZoomY");
    
    repaint();
}


void Polytempo_PositionRulerComponent::paint(Graphics& g)
{
    Polytempo_Sequence* sequence;
    
    if(!(sequence = Polytempo_Composition::getInstance()->getSelectedSequence())) return;

    //g.fillAll(Colour (235,235,235));
 
    g.setColour(sequence->getColour());
    g.fillRect(0,0,10,getHeight());
    
    //g.setColour(Colour(0,0,0));
    g.setFont(12);
    
    g.setColour(Colour(50,50,50));
    Polytempo_Event* event;
    Rational pos, labelPos = 0;
    int labelOffset = 0;
    String label;
    float y, thickness;
    int i = -1;
    while((event = sequence->getEvent(++i)) != nullptr)
    {
        pos = event->getPosition();
        if(event->getType() == eventType_Beat)
        {
            y = getHeight() - TIMEMAP_OFFSET - pos * zoomY;
            if(int(event->getProperty(eventPropertyString_Pattern)) < 20)
            {
                thickness = int(event->getProperty(eventPropertyString_Pattern)) == 10 ? 2.0 : 1.0;

                g.drawLine(45, y, getWidth(), y, thickness);
            }
            else
            {
                thickness = 0.5;
                g.drawLine(50, y, getWidth(), y, thickness);
            }
        }
        else if(event->getType() == eventType_Marker)
        {
            if(pos != labelPos)
            {
                labelPos = pos;
                labelOffset = 0;
            }
            label = event->getProperty("value");
            g.drawFittedText(label, 18+labelOffset, getHeight() - TIMEMAP_OFFSET - 6 - pos * zoomY, 10, 10, Justification::left, 1, 0.1);
            
            labelOffset += 12;
        }
    }
}

//------------------------------------------------------------------------------
#pragma mark -
#pragma mark tempo ruler

Polytempo_TempoRuler::Polytempo_TempoRuler()
{
    setScrollBarsShown(false, false, true, true);
    setViewedComponent(&rulerComponent, false);
    rulerComponent.setBounds(Rectangle<int>(65, 10000));
}

float Polytempo_TempoRuler::getIncrementForZoom(float zoom)
{
    if(zoom > 8000) return 0.0025;
    if(zoom > 4000) return 0.005;
    if(zoom > 2000) return 0.01;
    if(zoom > 1000) return 0.02;
    if(zoom >  500) return 0.04;
    return 0.08;
}


Polytempo_TempoRulerComponent::Polytempo_TempoRulerComponent()
{
    Polytempo_StoredPreferences::getInstance()->getProps().addChangeListener(this);
    zoomY = Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("tempoMapZoomY");
}

void Polytempo_TempoRulerComponent::changeListenerCallback(ChangeBroadcaster *)
{
    zoomY = Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("tempoMapZoomY");
    
    setSize(getWidth(), zoomY * 2);
}

void Polytempo_TempoRulerComponent::paint(Graphics& g)
{
    Polytempo_Sequence* sequence;
    
    if(!(sequence = Polytempo_Composition::getInstance()->getSelectedSequence())) return;
    
    g.setColour(sequence->getColour());
    g.fillRect(0,0,10,getHeight());
    
    g.setFont(12);
    g.setColour(Colour(50,50,50));
    
    float increment = Polytempo_TempoRuler::getIncrementForZoom(zoomY);
    float tempo = 0;
    int i = 0;
    
    while(tempo < 5.0f)
    {
        float y = getHeight() - TIMEMAP_OFFSET - tempo * zoomY;
        
        if(i++ % 2 == 0)
        {
            g.drawLine(55, y, getWidth(), y, 0.5);
            float num = Polytempo_TempoMeasurement::decodeTempoForUI(tempo);
            int decimalPlaces = num < 10 ? 4 : num < 100 ? 3 : num < 1000 ? 2 : 1;
            g.drawFittedText(String(num, decimalPlaces), 18, y - 6, 30, 10, Justification::left, 1);
        }
        else
        {
            g.drawLine(60, y, getWidth(), y, 0.5);
        }
        
        tempo += increment;
    }
}

