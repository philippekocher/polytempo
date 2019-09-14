/* ==============================================================================
 
 This file is part of the POLYTEMPO software package.
 Copyright (c) 2016 - Zurich University of the Arts,
 ICST Institute for Computer Music and Sound Technology
 http://www.icst.net
 
 Author: Philippe Kocher
 
 POLYTEMPO is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 POLYTEMPO is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this software. If not, see <http://www.gnu.org/licenses/>.
 
 ============================================================================== */

#ifndef __Polytempo_ComposerWindow__
#define __Polytempo_ComposerWindow__

#include "../JuceLibraryCode/JuceHeader.h"
#include "../../Views/PolytempoComposer/Polytempo_ComposerMainView.h"
#include "../../Views/PolytempoComposer/Polytempo_GraphicExportView.h"

class Polytempo_ComposerMenuBarModel;

struct Polytempo_LookAndFeel : public LookAndFeel_V3
{
    void drawTableHeaderBackground (Graphics& g, TableHeaderComponent&)
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
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_ComposerWindow)
};



#endif  // __Polytempo_ComposerWindow__
