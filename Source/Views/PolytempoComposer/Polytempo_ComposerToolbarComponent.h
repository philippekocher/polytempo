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

#ifndef __Polytempo_ComposerToolbarComponent__
#define __Polytempo_ComposerToolbarComponent__


#include "../../JuceLibraryCode/JuceHeader.h"
#include "../../Misc/Polytempo_Button.h"


class Polytempo_ComposerToolbarComponent : public Component,
                                           public Button::Listener
{
public:
    Polytempo_ComposerToolbarComponent();
    ~Polytempo_ComposerToolbarComponent();

    void paint (Graphics&);
    void resized();

    void buttonClicked(Button*);
    
    void setComponentTypes(int left, int right);

private:
    std::unique_ptr<Polytempo_Button> showTimeMapButton;
    std::unique_ptr<Polytempo_Button> showTempoMapButton;
    std::unique_ptr<Polytempo_Button> showBeatPatternListButton;
    std::unique_ptr<Polytempo_Button> showPointEditorButton;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_ComposerToolbarComponent)
};


#endif  // __Polytempo_ComposerToolbarComponent__
