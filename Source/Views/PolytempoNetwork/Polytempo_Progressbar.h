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

#ifndef __Polytempo_Progressbar__
#define __Polytempo_Progressbar__

#include "../../Scheduler/Polytempo_EventObserver.h"

class Polytempo_Progressbar : public Component,
                              public Polytempo_EventObserver
{
public:
    Polytempo_Progressbar();
    ~Polytempo_Progressbar();
    
    void paint(Graphics& g);
    
    void setText(String text);
    void setTime(float time);
    void setDuration(int duration);
    
    void eventNotification(Polytempo_Event *event);

    
private:
    ScopedPointer < String > text;
    float  time, duration;
    float  elapsedTime;
    OwnedArray < class Rectangle<float> > regionBounds;
};



#endif  // __Polytempo_Progressbar__
