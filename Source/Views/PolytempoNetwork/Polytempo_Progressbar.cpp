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

#include "Polytempo_Progressbar.h"
#include "Polytempo_GraphicsViewRegion.h"

Polytempo_Progressbar::Polytempo_Progressbar()
{
    setOpaque(false);
    
    elapsedTime = 0.0;
}

Polytempo_Progressbar::~Polytempo_Progressbar()
{}

void Polytempo_Progressbar::paint(Graphics &g)
{
    Rectangle<int>bounds = getLocalBounds();
    
    // text above
    bounds.setHeight(bounds.getHeight() * 0.5);
    g.setFont(24.0f);
    g.setColour(Colours::black);
    g.drawFittedText(text->toRawUTF8(),
                     bounds,  // inset rect
                     Justification::topLeft, 1);
    
    // text in progress bar
    bounds.setY(bounds.getHeight());
    g.setFont(16.0f);
    g.drawFittedText(String(duration*elapsedTime,2) + " s",
                     bounds.reduced(2,2),  // inset rect
                     Justification::centred, 1);

    // progress bar
    g.setColour(Colours::grey);
    g.drawRect(bounds);
    
    bounds.setWidth(bounds.getWidth() * elapsedTime);
    g.setColour(Colours::blue.withAlpha(0.2f));
    g.fillRect(bounds);
}

void Polytempo_Progressbar::setText(String text_)       { text = new String(text_); }
void Polytempo_Progressbar::setTime(float time_)        { time = time_; }
void Polytempo_Progressbar::setDuration(int duration_)  { duration = duration_; }


void Polytempo_Progressbar::eventNotification(Polytempo_Event *event)
{
    // update progress bar on every tick
    if(event->getType() == eventType_Tick)
    {
        if(event->getTime() == time) elapsedTime = 0;
        else  elapsedTime = float(event->getTime() - time) / duration;
        repaint();
    }
    // update progress bar when jumping to a locator
    else if(event->getType() == eventType_GotoLocator)
    {
        if(int(event->getProperty("value")) == time) elapsedTime = 0;
        else  elapsedTime = float(int(event->getProperty("value")) - time) / duration;
        repaint();
    }
    
    // remove when time is over
    if(elapsedTime >= 1)
    {
        ((Polytempo_GraphicsViewRegion*)getParentComponent())->clear();
    }
}