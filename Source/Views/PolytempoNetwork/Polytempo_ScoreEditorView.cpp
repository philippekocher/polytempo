/*
  ==============================================================================

    Polytempo_ScoreEditorView.cpp
    Created: 30 Nov 2018 10:19:37am
    Author:  Philippe Kocher

  ==============================================================================
*/

#include "Polytempo_ScoreEditorView.h"


Polytempo_ScoreEditorView::Polytempo_ScoreEditorView()
{}

Polytempo_ScoreEditorView::~Polytempo_ScoreEditorView()
{
    deleteAllChildren();
}

void Polytempo_ScoreEditorView::paint(Graphics& g)
{
    g.fillAll(Colours::white);
}
