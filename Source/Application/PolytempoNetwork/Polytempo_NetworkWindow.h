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

#pragma once

#include "../../Views/PolytempoNetwork/Polytempo_PageEditorView.h"
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
    };
    
    void setContentID(contentID);
    int  getContentID();
    
    Component* getContentComponent();

private:
    ScopedPointer <Polytempo_NetworkMainView> mainView;
    ScopedPointer <Polytempo_PageEditorView> pageEditorView;

    OpenGLContext openGLContext;
    
    contentID currentContentID = mainViewID;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_NetworkWindow)
};
