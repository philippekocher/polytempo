#pragma once

#include "JuceHeader.h"
#include "Polytempo_NetworkView.h"
#include "../../Data/Polytempo_Score.h"
#include "../../Scheduler/Polytempo_EventObserver.h"

class Polytempo_ScoreEditorView : public Component, public Polytempo_NetworkView, public Polytempo_EventObserver
{
public:
    Polytempo_ScoreEditorView();
    ~Polytempo_ScoreEditorView() override;

    void paint(Graphics&) override;
    void resized() override;

    void refresh();
    bool applyChanges();

    void eventNotification(Polytempo_Event*) override;

private:
    CodeDocument codeDocument;
    std::unique_ptr<CodeEditorComponent> editor;

    Polytempo_Score* score;
};
