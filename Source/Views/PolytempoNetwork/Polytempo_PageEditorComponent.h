#pragma once

#include "../../Scheduler/Polytempo_Event.h"
#include "../../Data/Polytempo_Score.h"
#include "../../Misc/DragHandle.h"

class Polytempo_PageEditorComponent : public Component,
                                      public ChangeListener,
                                      public DragHandleListener
{
public:
    Polytempo_PageEditorComponent();
    ~Polytempo_PageEditorComponent();
    
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
