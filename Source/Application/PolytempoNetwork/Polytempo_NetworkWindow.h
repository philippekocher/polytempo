#pragma once

#include "../../Views/PolytempoNetwork/Polytempo_PageEditorView.h"
#include "../../Views/PolytempoNetwork/Polytempo_RegionEditorView.h"
#include "../../Views/PolytempoNetwork/Polytempo_ScoreEditorView.h"
#include "../../Views/PolytempoNetwork/Polytempo_NetworkMainView.h"

class Polytempo_NetworkWindow : public DocumentWindow
{
public:
    Polytempo_NetworkWindow();
    ~Polytempo_NetworkWindow();

    void closeButtonPressed();
    
    enum contentID
    {
        mainViewID = 0,
        pageEditorViewID,
        regionEditorViewID,
        scoreEditorViewID,
    };
    
    void setContentID(contentID);
    int  getContentID();
    
    Component* getContentComponent();
	void performSetContentID();
    
    bool applyChanges();

private:
	std::unique_ptr <Polytempo_NetworkMainView> mainView;
	std::unique_ptr <Polytempo_PageEditorView> pageEditorView;
	std::unique_ptr <Polytempo_RegionEditorView> regionEditorView;
	std::unique_ptr <Polytempo_ScoreEditorView> scoreEditorView;

    OpenGLContext openGLContext;
    
    contentID currentContentID = mainViewID;
    contentID contentIdToBeSet = mainViewID;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_NetworkWindow)
};
