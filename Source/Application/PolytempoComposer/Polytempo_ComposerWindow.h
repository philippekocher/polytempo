#pragma once

#include "JuceHeader.h"
#include "../../Views/PolytempoComposer/Polytempo_ComposerMainView.h"
#include "../../Views/PolytempoComposer/Polytempo_GraphicExportView.h"

class Polytempo_ComposerMenuBarModel;

struct Polytempo_LookAndFeel : public LookAndFeel_V3
{
    void drawTableHeaderBackground (Graphics& g, TableHeaderComponent&) override
    {
        g.fillAll(Colour(245,245,245));
    }
    
    void drawTableHeaderColumn (Graphics& g, const String& columnName, int /*columnId*/,
                                                int width, int height, bool /*isMouseOver*/, bool /*isMouseDown*/,
                                                int /*columnFlags*/)
    {
        Rectangle<int> area(width, height);
        area.reduce(2, 0);
        
        g.setColour(Colours::black);
        g.setFont(Font (height * 0.5f, Font::plain));
        g.drawFittedText(columnName, area, Justification::centredLeft, 1);
    }
};


class Polytempo_ComposerWindow : public DocumentWindow
{
public:
    Polytempo_ComposerWindow();
    ~Polytempo_ComposerWindow();

    void closeButtonPressed();

    enum contentID
    {
        mainViewID = 0,
        graphicExportViewID,
    };
    
    void setContentID(contentID);
    int  getContentID();

    String getWindowContentStateAsString();
    void   restoreWindowContentStateFromString(const String&);
    
    Polytempo_ComposerMainView* getMainView() { return mainView.get(); }
    
private:
    std::unique_ptr<Polytempo_ComposerMenuBarModel> menuBarModel;
    std::unique_ptr<Polytempo_ComposerMainView> mainView;
    std::unique_ptr<Polytempo_GraphicExportView> graphicExportView;
    Polytempo_LookAndFeel lookAndFeel;
    
    OpenGLContext openGLContext;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_ComposerWindow)
};
