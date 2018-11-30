/*
  ==============================================================================

    Polytempo_ScoreEditorView.h
    Created: 30 Nov 2018 10:19:37am
    Author:  Philippe Kocher

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

class Polytempo_ScoreEditorView : public Component
{
public:
    Polytempo_ScoreEditorView();
    ~Polytempo_ScoreEditorView();
    
    void paint(Graphics&) override;
};
