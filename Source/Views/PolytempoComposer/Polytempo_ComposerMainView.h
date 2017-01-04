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

#ifndef __Polytempo_ComposerMainView__
#define __Polytempo_ComposerMainView__

#include "../JuceLibraryCode/JuceHeader.h"
#include "Polytempo_ComposerToolbarComponent.h"
#include "Polytempo_TransportComponent.h"
#include "Polytempo_SequencesViewport.h"
#include "Polytempo_TimeMapComponent.h"
#include "Polytempo_TempoMapComponent.h"
#include "Polytempo_BeatPatternListComponent.h"
#include "Polytempo_PointListComponent.h"



class Polytempo_ComposerMainView : public Component
{
public:
    Polytempo_ComposerMainView();
    ~Polytempo_ComposerMainView();

    void paint (Graphics&);
    void resized();
    void childBoundsChanged(Component* child);
    
    String getComponentStateAsString();
    void   restoreComponentStateFromString(const String&);
    
    enum componentType
    {
        componentType_None = 0,
        componentType_TimeMap,
        componentType_TempoMap,
        componentType_PatternList,
        componentType_PointList
    };
    
    
    void setLeftComponent(componentType type);
    componentType getLeftComponent();
    void setRightComponent(componentType type);
    
private:
    ScopedPointer < Polytempo_ComposerToolbarComponent >  toolbarComponent;
    ScopedPointer < Polytempo_TransportComponent >        transportComponent;
    Polytempo_SequencesViewport        sequencesViewport;
    
    Component                           leftComponent;      // split view
    Component                           rightComponent;
    
    ScopedPointer < Polytempo_TimeMapComponent >    timeMapComponent;
    ScopedPointer < Polytempo_TempoMapComponent >   tempoMapComponent;
    
    Polytempo_BeatPatternListComponent  beatPatternListComponent;
    Polytempo_PointListComponent        pointListComponent;
    
    StretchableLayoutManager    stretchableManager;
    ScopedPointer < StretchableLayoutResizerBar > resizerBar;
    
    float resizerBarPosition;
    componentType leftComponentType = componentType_None;
    componentType rightComponentType = componentType_None;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_ComposerMainView)
};


#endif  // __Polytempo_ComposerMainComponent__
