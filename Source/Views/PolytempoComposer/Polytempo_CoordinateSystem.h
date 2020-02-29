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


#ifndef __Polytempo_CoordinateSystem__
#define __Polytempo_CoordinateSystem__


#include "../JuceLibraryCode/JuceHeader.h"
#include "../../Data/Polytempo_Composition.h"
#include "../../Scheduler/Polytempo_EventObserver.h"


class Polytempo_CoordinateSystemComponent : public Component,
                                            public Polytempo_EventObserver
{
public:
    Polytempo_CoordinateSystemComponent();
    ~Polytempo_CoordinateSystemComponent();
    
    void setSizeAndZooms(int w, int h, float zX, float zY);
    void eventNotification(Polytempo_Event*);

    void mouseUp(const MouseEvent &);
    
protected:
    float zoomX, zoomY;
    Viewport *viewport;
    
    Rectangle<float> selectionRectangle;
    OwnedArray<Polytempo_ControlPoint> draggedControlPointsOrigin;
    std::unique_ptr<Polytempo_ControlPoint> draggedPoint;
    bool hit;
        
    std::unique_ptr<DrawableRectangle> playhead;
    std::unique_ptr<Array<float>> horizontalGrid;
};

class Polytempo_TimeMapCoordinateSystem : public Polytempo_CoordinateSystemComponent
{
public:
    Polytempo_TimeMapCoordinateSystem(Viewport*);
    void paint(Graphics&);
private:
    void paintSequence(Graphics&, Polytempo_Sequence*, bool selected);
public:
    void mouseDown(const MouseEvent &);
    void mouseDrag(const MouseEvent &);
};

class Polytempo_TempoMapCoordinateSystem : public Polytempo_CoordinateSystemComponent
{
public:
    Polytempo_TempoMapCoordinateSystem(Viewport*);
    void paint(Graphics&);
private:
    void paintSequence(Graphics&, Polytempo_Sequence*, bool selected);
public:
    void mouseDown(const MouseEvent &);
    void mouseDrag(const MouseEvent &);
};

class Polytempo_CoordinateSystem : public Viewport
{
public:
    Polytempo_CoordinateSystem();
    ~Polytempo_CoordinateSystem();
    void visibleAreaChanged(const Rectangle< int > &r);
    void setSynchronizedViewport(Viewport *sync, int axis);
    
    void showPopupMenu();
private:
    std::unique_ptr <Polytempo_CoordinateSystemComponent> coordinateSystemComponent;
    Viewport* synchronizedViewport[2] = {nullptr, nullptr};
};


#endif  // __Polytempo_CoordinateSystem__
