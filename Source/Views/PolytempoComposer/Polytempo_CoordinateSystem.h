#pragma once

#include "JuceHeader.h"
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
    bool keyPressed(const KeyPress &);
    
protected:
    float zoomX, zoomY;
    Viewport *viewport;
    
    Rectangle<float> selectionRectangle;
    OwnedArray<Polytempo_ControlPoint> draggedControlPointsOrigin;
    Polytempo_ControlPoint *draggedPoint;
    bool hit;
        
    std::unique_ptr<DrawableRectangle> playhead;
};

class Polytempo_TimeMapCoordinateSystem : public Polytempo_CoordinateSystemComponent
{
public:
    Polytempo_TimeMapCoordinateSystem(Viewport*);
    void paint(Graphics&);
private:
    void paintSequence(Graphics&, Polytempo_Sequence*, bool selected);
    Rational quantiseMousePosition(float);
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
