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

#ifndef __Polytempo_TransportComponent__
#define __Polytempo_TransportComponent__

#include "../../JuceLibraryCode/JuceHeader.h"
#include "../../Misc/Polytempo_Button.h"
#include "../../Misc/Polytempo_Textbox.h"
#include "../../Scheduler/Polytempo_EventObserver.h"


class Polytempo_TransportComponent : public Component,
                                     public Button::Listener,
                                     public Label::Listener,
                                     public Polytempo_EventObserver
{
public:
    Polytempo_TransportComponent();
    ~Polytempo_TransportComponent();
    
    void paint (Graphics&);
    void resized();
    
    void buttonClicked(Button*);
    void editorShown(Label*, TextEditor&);
    void labelTextChanged(Label*);
    
    void eventNotification(Polytempo_Event*);
    
private:
    ScopedPointer<Polytempo_Button> startButton;
    ScopedPointer<Polytempo_Button> returnToLocatorButton;
    ScopedPointer<Polytempo_Button> returnToZeroButton;
    
    Polytempo_Textbox *timeTextbox;
    String timeString;
    
    Polytempo_Textbox *tempoFactorTextbox;
    String tempoFactorString;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_TransportComponent)
};



#endif  // __Polytempo_TransportComponent__
