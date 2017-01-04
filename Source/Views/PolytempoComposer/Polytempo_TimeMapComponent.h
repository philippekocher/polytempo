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

#ifndef __Polytempo_TimeMapComponent__
#define __Polytempo_TimeMapComponent__

#include "../JuceLibraryCode/JuceHeader.h"
#include "Polytempo_CoordinateSystem.h"
#include "Polytempo_Ruler.h"


class Polytempo_TimeMapComponent : public Component
{
public:
    Polytempo_TimeMapComponent();
    ~Polytempo_TimeMapComponent();

    void paint (Graphics&);
    void resized();

private:
    ScopedPointer < Polytempo_TimeMapCoordinateSystem > timeMapCoordinateSystem;
    ScopedPointer < Polytempo_CoordinateSystem >  coordinateSystem;
    Polytempo_TimeRuler         timeRuler;
    Polytempo_PositionRuler     positionRuler;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_TimeMapComponent)
};


#endif  // __Polytempo_TimeMapComponent__
