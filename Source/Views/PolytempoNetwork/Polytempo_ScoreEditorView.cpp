#include "Polytempo_ScoreEditorView.h"
#include "../../Application/PolytempoNetwork/Polytempo_NetworkApplication.h"
#include "../../Misc/Polytempo_Alerts.h"


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

void Polytempo_ScoreEditorView::paint(Graphics&)
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

static void discardChangesCallback(int result, Polytempo_ScoreEditorView* parent)
{
    if(result) parent->refresh();
}

bool Polytempo_ScoreEditorView::applyChanges()
{
    if(!editor->getDocument().hasChangedSinceSavePoint()) return true;
    
    bool result = score->setJsonString(editor->getDocument().getAllContent());
    
    if(!result)
    {
        Polytempo_OkCancelAlert::show("Error",
                                      "JSON code is not well-formed. Do you want to discard the changes?",
                                      ModalCallbackFunction::create(discardChangesCallback, this));
    }
    else
    {
        editor->getDocument().setSavePoint();
    }
    
    return result;
}

//------------------------------------------------------------------------------
#pragma mark -
#pragma mark event observer

void Polytempo_ScoreEditorView::eventNotification(Polytempo_Event *event)
{
    if(event->getType() == eventType_DeleteAll && isVisible())
        refresh();
    if(event->getType() == eventType_Ready && isVisible())
        refresh();
}
