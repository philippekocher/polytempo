/*
  ==============================================================================

    Polytempo_ScoreEditorView.h
    Created: 30 Nov 2018 10:19:37am
    Author:  Philippe Kocher

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "../../Data/Polytempo_Score.h"
#include "../../Scheduler/Polytempo_EventObserver.h"


class Polytempo_ScoreEditorView : public Component, public Polytempo_EventObserver
{
public:
    Polytempo_ScoreEditorView();
    ~Polytempo_ScoreEditorView();
    
    void paint(Graphics&) override;
    void resized() override;
    
    void refresh();
    bool applyChanges();
    
    void eventNotification(Polytempo_Event*) override;
    
private:
    CodeDocument codeDocument;
    ScopedPointer<CodeEditorComponent> editor;

    Polytempo_Score *score;
};
