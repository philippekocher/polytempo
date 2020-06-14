#pragma once

#include "JuceHeader.h"


class Polytempo_RulerComponent : public Component
{
public:
    void setSizeAndZooms(int w, int h, float zX, float zY);
protected:
    float zoomX, zoomY;
};

class Polytempo_TimeRulerComponent : public Polytempo_RulerComponent
{
public:
    Polytempo_TimeRulerComponent();
    void paint(Graphics&);
};

class Polytempo_PositionRulerComponent : public Polytempo_RulerComponent
{
public:
    Polytempo_PositionRulerComponent();
    void paint(Graphics&);
};


class Polytempo_Ruler : public Viewport
{
public:
    Polytempo_Ruler();
    ~Polytempo_Ruler();
    void visibleAreaChanged(const Rectangle< int > &r);
    void setSynchronizedViewport(Viewport *sync, int axis);
    void setSizeAndZooms(int w, int h, float zX, float zY);
protected:
    bool horizontal = true;
    Viewport *synchronizedViewport = nullptr;
    int synchronizedAxis;
    std::unique_ptr<Polytempo_RulerComponent> rulerComponent;
};

class Polytempo_TimeRuler : public Polytempo_Ruler
{
public:
    Polytempo_TimeRuler();
    static int getIncrementForZoom(float zoomX);
};

class Polytempo_PositionRuler : public Polytempo_Ruler
{
public:
    Polytempo_PositionRuler();
};

class Polytempo_TempoRulerComponent : public Polytempo_RulerComponent
{
public:
    Polytempo_TempoRulerComponent();
    void paint(Graphics&);
};

class Polytempo_TempoRuler : public Polytempo_Ruler
{
public:
    Polytempo_TempoRuler();
    static float getIncrementForZoom(float zoomX);
};
