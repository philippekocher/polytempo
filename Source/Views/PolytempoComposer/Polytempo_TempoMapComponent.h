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

#ifndef __Polytempo_TempoMapComponent__
#define __Polytempo_TempoMapComponent__

#include "../JuceLibraryCode/JuceHeader.h"
#include "Polytempo_CoordinateSystem.h"
#include "Polytempo_Ruler.h"


class Polytempo_TempoMapComponent : public Component
{
public:
    Polytempo_TempoMapComponent();
    ~Polytempo_TempoMapComponent();

    void paint (Graphics&);
    void resized();

private:
    ScopedPointer < Polytempo_TempoMapCoordinateSystem > tempoMapCoordinateSystem;
    ScopedPointer < Polytempo_CoordinateSystem >  coordinateSystem;
    Polytempo_TimeRuler         timeRuler;
    Polytempo_TempoRuler        tempoRuler;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_TempoMapComponent)
};


#endif  // __Polytempo_TimeMapComponent__
