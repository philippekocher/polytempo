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

#include "JuceHeader.h"
#include "../../Data/Polytempo_Score.h"
#include "../../Scheduler/Polytempo_EventObserver.h"
#include "../../Misc/DragHandle.h"
#include "../../Misc/Polytempo_Textbox.h"


class Polytempo_RegionEditorView : public Component,
                                   public DragHandleListener,
                                   public Label::Listener,
                                   public Polytempo_EventObserver
{
public:
    Polytempo_RegionEditorView();
    ~Polytempo_RegionEditorView();
    
    void paint(Graphics&) override;
    void resized() override;
    void refresh();
    
    bool keyPressed(const KeyPress&) override;
    void mouseDown(const MouseEvent&) override;
    
    void addRegion();

    // Drag Handle Listener
    void positionChanged(DragHandle*) override;
    void draggingSessionEnded() override;
    
    // Label Listener
    void editorShown(Label* label, TextEditor&) override;
    void labelTextChanged(Label* labelThatHasChanged) override;
    
    // Event Observer
    void eventNotification(Polytempo_Event*) override;
    
private:
    void setSelectedRegionID(int);
    void updateSelectedRegion(Array<var>);

    Polytempo_Score *score;
    Array < DragHandle* > dragHandles;    
    Array < Polytempo_Event* > addRegionEvents;
    int selectedRegionID;
    
    Label *relativePositionLabel;
    Polytempo_Textbox *xTextbox;
    Polytempo_Textbox *yTextbox;
    Polytempo_Textbox *wTextbox;
    Polytempo_Textbox *hTextbox;
};
