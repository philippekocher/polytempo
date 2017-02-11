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

#ifndef __DragHandle__
#define __DragHandle__

//#include "../JuceLibraryCode/JuceHeader.h"

class DragHandle;

class DragHandleListener
{
public:
    virtual ~DragHandleListener() {};
    virtual void draggingSessionEnded() = 0;
    virtual void positionChanged(DragHandle*) = 0;
};

class DragHandle : public Component
{
public:
    DragHandle(DragHandleListener* theListener)
    : listener(theListener)
    {
        setSize (15, 15);
        setRepaintsOnMouseActivity (true);
    }
    
    void paint(Graphics& g) override
    {
        g.setColour(Colours::white);
        g.fillRect(getLocalBounds().reduced (3).toFloat());
    
        g.setColour(Colours::black);
        g.drawRect(getLocalBounds().reduced (3).toFloat(), isMouseOverOrDragging() ? 1.5f : 1.0f);
    }
    
    void setConstraint(int c)
    {
        constraint = c;
    }
    
    void setBoundsConstraint(Rectangle < int > r)
    {
        boundsConstraint = r;
    }
    
    void mouseDown(const MouseEvent&) override
    {
        originalPosition = Point<float>(getCentreX(), getCentreY());
    }

    void mouseDrag(const MouseEvent& e) override
    {
        Point <int> point = e.getEventRelativeTo(getParentComponent()).getPosition();
        
        if(boundsConstraint.getWidth() != 0 && boundsConstraint.getHeight() != 0)
        {
            point = boundsConstraint.getConstrainedPoint(point);
        }
        
        if(constraint == 1)
        {
            setCentrePosition(point.x, (int)getCentreY());
        }
        else if(constraint == 2)
        {
            setCentrePosition((int)getCentreX(), point.y);
        }
        else
        {
            setCentrePosition(point.x, point.y);
        }
        
        if(listener) listener->positionChanged(this);
    }
    
    void mouseUp(const MouseEvent&) override
    {
        if(listener) listener->draggingSessionEnded();
    }
    
    float getCentreX()
    {
        return getX() + getWidth() / 2.0f;
    }
    
    float getCentreY()
    {
        return getY() + getHeight() / 2.0f;
    }
    
private:
    
    DragHandleListener* listener;
    Point<float> originalPosition;
    Rectangle<int> boundsConstraint;
    int constraint = 0; // 0=no constraint, 1=horizontal only, 2=vertical only
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DragHandle)
};


#endif  // __Handle__
