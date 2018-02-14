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

#pragma once

#include "JuceHeader.h"
#include "../../Data/Polytempo_Score.h"
#include "../../Scheduler/Polytempo_EventObserver.h"
#include "../../Misc/DragHandle.h"


class Polytempo_RegionEditorView : public Component,
                                   public DragHandleListener,
                                   public Polytempo_EventObserver
{
public:
    Polytempo_RegionEditorView();
    ~Polytempo_RegionEditorView();
    
    void paint(Graphics&);
    void refresh();
    
    bool keyPressed(const KeyPress&);
    void mouseDown(const MouseEvent&);

    // Drag Handle Listener
    void positionChanged(DragHandle*);
    void draggingSessionEnded();
    
    // Event Observer
    void eventNotification(Polytempo_Event*);
    
private:
    Polytempo_Score *score;
    Array < DragHandle* > dragHandles;    
    Array < Polytempo_Event* > addRegionEvents;
    int selectedRegionID;
};
