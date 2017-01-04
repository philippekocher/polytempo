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


#ifndef __Polytempo_ImageEditorComponent__
#define __Polytempo_ImageEditorComponent__

//#include "../../JuceLibraryCode/JuceHeader.h"
#include "../../Scheduler/Polytempo_Event.h"
#include "../../Data/Polytempo_Score.h"
#include "../../Misc/DragHandle.h"


class Polytempo_ImageEditorComponent : public Component,
                                       public ChangeListener,
                                       public DragHandleListener
{
public:
    Polytempo_ImageEditorComponent();
    ~Polytempo_ImageEditorComponent();
    
    void update();
    void paint(Graphics&);
    void resized();
    void setImage(Image*);
    void setSectionRect(Rectangle < float >);
    void setEditedEvent(Polytempo_Event*);
    
    // Drag Handle Listener
    void positionChanged(DragHandle*);
    void draggingSessionEnded();
    
    void changeListenerCallback(ChangeBroadcaster* bc);
    
private:
    Image *image;
    float zoomFactor;

    Rectangle < float > sectionRect;
    Array < DragHandle* > dragHandles;
    
    Polytempo_Event *editedEvent;
    Polytempo_Score *score;
};



#endif  // __Polytempo_ImageEditorComponent__
