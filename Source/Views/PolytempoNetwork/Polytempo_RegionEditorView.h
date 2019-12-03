#pragma once

#include "JuceHeader.h"
#include "../../Data/Polytempo_Score.h"
#include "../../Scheduler/Polytempo_EventObserver.h"
#include "../../Misc/DragHandle.h"
#include "../../Misc/Polytempo_Textbox.h"

class Polytempo_RegionEditorView : public Component,
                                   public DragHandleListener,
                                   public Label::Listener,
                                   public Polytempo_EventObserver
{
public:
    Polytempo_RegionEditorView();
    ~Polytempo_RegionEditorView();

    void paint(Graphics&) override;
    void resized() override;
    void refresh();

    bool keyPressed(const KeyPress&) override;
    void mouseDown(const MouseEvent&) override;

    void addRegion();

    // Drag Handle Listener
    void positionChanged(DragHandle*) override;
    void draggingSessionEnded() override;

    // Label Listener
    void editorShown(Label* label, TextEditor&) override;
    void labelTextChanged(Label* labelThatHasChanged) override;

    // Event Observer
    void eventNotification(Polytempo_Event*) override;

private:
    void setSelectedRegionID(int);
    void updateSelectedRegion(Array<var>);

    Polytempo_Score* score;
    Array<DragHandle*> dragHandles;
    Array<Polytempo_Event*> addRegionEvents;
    int selectedRegionID;

    Label* relativePositionLabel;
    Polytempo_Textbox* xTextbox;
    Polytempo_Textbox* yTextbox;
    Polytempo_Textbox* wTextbox;
    Polytempo_Textbox* hTextbox;
};
