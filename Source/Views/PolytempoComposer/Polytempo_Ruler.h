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

#ifndef __Polytempo_Ruler__
#define __Polytempo_Ruler__

#include "../JuceLibraryCode/JuceHeader.h"


class Polytempo_RulerComponent : public Component,
                                 public ChangeListener
{
protected:
    float zoomX, zoomY;
};

class Polytempo_TimeRulerComponent : public Polytempo_RulerComponent
{
public:
    Polytempo_TimeRulerComponent();
    void changeListenerCallback (ChangeBroadcaster*);
    void paint(Graphics&);
};

class Polytempo_PositionRulerComponent : public Polytempo_RulerComponent
{
public:
    Polytempo_PositionRulerComponent();
    void changeListenerCallback (ChangeBroadcaster*);
    void paint(Graphics&);
};


class Polytempo_Ruler : public Viewport
{
public:
    Polytempo_Ruler();
    ~Polytempo_Ruler();
    void visibleAreaChanged(const Rectangle< int > &r);
    void setSynchronizedViewport(Viewport *sync, int axis);
protected:
    bool horizontal = true;
    Viewport *synchronizedViewport = nullptr;
    int synchronizedAxis;
};

class Polytempo_TimeRuler : public Polytempo_Ruler
{
public:
    Polytempo_TimeRuler();
    static int getIncrementForZoom(float zoomX);
private:
    Polytempo_TimeRulerComponent rulerComponent;
};

class Polytempo_PositionRuler : public Polytempo_Ruler
{
public:
    Polytempo_PositionRuler();
private:
    Polytempo_PositionRulerComponent rulerComponent;
};


class Polytempo_TempoRulerComponent : public Polytempo_RulerComponent
{
public:
    Polytempo_TempoRulerComponent();
    void changeListenerCallback (ChangeBroadcaster*);
    void paint(Graphics&);
};

class Polytempo_TempoRuler : public Polytempo_Ruler
{
public:
    Polytempo_TempoRuler();
private:
    Polytempo_TempoRulerComponent rulerComponent;
};





#endif  // __Polytempo_Ruler__
