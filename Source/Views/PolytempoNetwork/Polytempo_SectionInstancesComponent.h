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
#include "../../Misc/Polytempo_Textbox.h"
#include "../../Scheduler/Polytempo_Event.h"
#include "../../Data/Polytempo_Score.h"

class Polytempo_PageEditorView;

class Polytempo_SectionInstancesComponent : public Component, public Label::Listener, public Button::Listener
{
public:
    Polytempo_SectionInstancesComponent();
    ~Polytempo_SectionInstancesComponent();
    
    void paint(Graphics&);
    void resized();

    void setModel(Polytempo_PageEditorView *);
    void setImageEvents(Array< Polytempo_Event* >&, var&);

    // Label Listener
    void editorShown(Label*, TextEditor&);
    void labelTextChanged(Label*);

    // Button Listener
    void buttonClicked(Button*);

private:
    Array < Polytempo_Textbox *> markerTextboxes;
    Array < Polytempo_Textbox *> timeTextboxes;
    Array < Polytempo_Textbox *> regionTextboxes;
    Array < TextButton *> deleteButtons;
    
    Array < Polytempo_Event *> imageEvents;

    void addTextboxes(int);

    Polytempo_Score *score;
    Polytempo_PageEditorView *pageEditorView;
};
