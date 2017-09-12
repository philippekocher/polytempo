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

#ifndef __Polytempo_AuxiliaryView__
#define __Polytempo_AuxiliaryView__

#include "../../Scheduler/Polytempo_EventObserver.h"
#include "../../Misc/Polytempo_Textbox.h"
#include "../../Misc/Polytempo_Button.h"
#include "Polytempo_GraphicsAnnotationManagerView.h"


class Polytempo_AuxiliaryView : public Component,
                                public Label::Listener,
                                public Button::Listener,
                                public Polytempo_EventObserver
{
public:
    Polytempo_AuxiliaryView();
    ~Polytempo_AuxiliaryView();

    void paint (Graphics&);
    void resized();
    void eventNotification(Polytempo_Event *event);

    /** Called when a Label goes into editing mode */
    void editorShown(Label* label, TextEditor&);
    void labelTextChanged(Label* labelThatHasChanged);
    
    void buttonClicked(Button *button);
    void buttonStateChanged(Button *button);
    
private:
    Polytempo_Textbox *markerTextbox;
    String            markerString;
    Polytempo_Button  *markerBackwards;
    Polytempo_Button  *markerForwards;
    Polytempo_Button  *imageBackwards;
    Polytempo_Button  *imageForwards;
    
    Polytempo_Textbox *timeTextbox;
    
//  Polytempo_Textbox *realTimeTextbox;
    
    Polytempo_Textbox *tempoFactorTextbox;
    String            tempoFactor;
    
    AttributedString peers;
    Label       *peersTextbox;
    
	Polytempo_GraphicsAnnotationManagerView *annotationManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_AuxiliaryView)
};


#endif  // __Polytempo_AuxiliaryView__
