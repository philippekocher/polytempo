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

#ifndef __Polytempo_NetworkMainView__
#define __Polytempo_NetworkMainView__

#include "../JuceLibraryCode/JuceHeader.h"

class Polytempo_VisualMetro;
class Polytempo_GraphicsView;
class Polytempo_AuxiliaryView;

//------------------------------------------------------------------------------
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class Polytempo_NetworkMainView : public Component,
                                  public ChangeListener
{
public:
    //------------------------------------------------------------------------------
    Polytempo_NetworkMainView();
    ~Polytempo_NetworkMainView();

    /* graphics
     --------------------------------------- */
    
    void paint (Graphics& g);
    void resized();

    /* slider & button listener
     --------------------------------------- */
    void changeListenerCallback (ChangeBroadcaster* source);

private:
    Polytempo_VisualMetro *visualMetro;
    Polytempo_GraphicsView *graphicsView;
    Polytempo_AuxiliaryView *auxiliaryView;
    
    //------------------------------------------------------------------------------
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_NetworkMainView)
};


#endif  // __Polytempo_NetworkMainView__
