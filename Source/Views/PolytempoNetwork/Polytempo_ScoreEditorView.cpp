/*
  ==============================================================================

    Polytempo_ScoreEditorView.cpp
    Created: 30 Nov 2018 10:19:37am
    Author:  Philippe Kocher

  ==============================================================================
*/

#include "Polytempo_ScoreEditorView.h"
#include "../../Application/PolytempoNetwork/Polytempo_NetworkApplication.h"


Polytempo_ScoreEditorView::Polytempo_ScoreEditorView()
{
    setOpaque(true);
    
    // Create the editor..
    editor.reset(new CodeEditorComponent(codeDocument, nullptr));
    addAndMakeVisible(editor.get());
}

Polytempo_ScoreEditorView::~Polytempo_ScoreEditorView()
{
    editor = nullptr;
}

void Polytempo_ScoreEditorView::paint(Graphics& g)
{}

void Polytempo_ScoreEditorView::resized()
{
    editor->setBounds(getLocalBounds());
}

void Polytempo_ScoreEditorView::refresh()
{
    
    Polytempo_NetworkApplication* const app = dynamic_cast<Polytempo_NetworkApplication*>(JUCEApplication::getInstance());
    score = app->getScore();
    
    if(score == nullptr) return;
    
    editor->loadContent(score->getJsonString());
}
