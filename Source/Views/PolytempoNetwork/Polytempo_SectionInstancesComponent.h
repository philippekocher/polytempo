#pragma once

#include "JuceHeader.h"
#include "../../Misc/Polytempo_Textbox.h"
#include "../../Scheduler/Polytempo_Event.h"
#include "../../Data/Polytempo_Score.h"

class Polytempo_PageEditorView;

class Polytempo_SectionInstancesComponent : public Component, public Label::Listener, public Button::Listener
{
public:
    Polytempo_SectionInstancesComponent();
    ~Polytempo_SectionInstancesComponent();

    void paint(Graphics&) override;
    void resized() override;

    void setModel(Polytempo_PageEditorView*);
    void setImageEvents(Array<Polytempo_Event*>&, var&);

    // Label Listener
    void editorShown(Label*, TextEditor&) override;
    void labelTextChanged(Label*) override;

    // Button Listener
    void buttonClicked(Button*) override;

private:
    Array<Polytempo_Textbox *> markerTextboxes;
    Array<Polytempo_Textbox *> timeTextboxes;
    Array<Polytempo_Textbox *> regionTextboxes;
    Array<TextButton *> deleteButtons;

    Array<Polytempo_Event *> imageEvents;

    void addTextboxes(int);

    Polytempo_Score* score;
    Polytempo_PageEditorView* pageEditorView;
};
