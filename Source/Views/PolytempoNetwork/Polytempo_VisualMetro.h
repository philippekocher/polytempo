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

#ifndef __Polytempo_VisualMetro__
#define __Polytempo_VisualMetro__



//#include "../../JuceLibraryCode/JuceHeader.h"
#include "../../Scheduler/Polytempo_EventObserver.h"

class Polytempo_VisualConductorComponent;

class Polytempo_VisualMetro : public  Component,
                              private Timer,
                              public  Polytempo_EventObserver
{
public:
    Polytempo_VisualMetro();
    ~Polytempo_VisualMetro();

    void paint (Graphics& g);
    
    void setForegroundColour (Colour& colour);
    void setBackgroundColour (Colour& colour);
    void setFrameColour (Colour& colour);
    void setWidth (float val);
    
    void timerCallback();
    void eventNotification(Polytempo_Event *event);

private:
    float width;
    float pos, subpos;
    float increment;
    int timeInterval;
    float x,y;
    float tempoFactor;
    float exponentMain, exponentSub;
    float beatDuration;
    int beatSubdivision, pattern;
    Colour foregroundColour;
    Colour normalColour, cueColour;
    bool shouldStop;
    bool linear;
    int holdMax;

    Polytempo_VisualConductorComponent *hComponent;
    Polytempo_VisualConductorComponent *vComponent;
};

#endif   // __Polytempo_VisualMetro__
