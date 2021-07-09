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
    ~Polytempo_PageEditorComponent() override;

    void update();
    void paint(Graphics&) override;
    void resized() override;
    void setImage(Image*);
    void setSectionRect(Rectangle<float>);
    void setEditedEvent(Polytempo_Event*);

    // Drag Handle Listener
    void positionChanged(DragHandle*) override;
    void draggingSessionEnded() override;

    void changeListenerCallback(ChangeBroadcaster* bc) override;

private:
    Image* image;
    float zoomFactor;

    Rectangle<float> sectionRect;
    Array<DragHandle*> dragHandles;

    Polytempo_Event* editedEvent;
    Polytempo_Score* score;
};
