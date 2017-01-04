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

#ifndef __Polytempo_BeatPatternListComponent__
#define __Polytempo_BeatPatternListComponent__


#include "../JuceLibraryCode/JuceHeader.h"
#include "Polytempo_ListComponent.h"


class Polytempo_BeatPatternListComponent : public Polytempo_ListComponent
{
public:
    Polytempo_BeatPatternListComponent();
    ~Polytempo_BeatPatternListComponent();

    void paint(Graphics&);

    String getText(int rowNumber, int columnID);
    void   setText(String text, int rowNumber, int columnID);

    int getNumRows();
    void selectedRowsChanged(int lastRowSelected);
    void paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected);
    Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component* existingComponentToUpdate);
    
    void showPopupMenu();

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_BeatPatternListComponent)
};


#endif  // __Polytempo_BeatPatternListComponent__
