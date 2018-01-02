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

#include "Polytempo_ListComponent.h"


void Polytempo_ListComponent::resized()
{
    Rectangle<int> r = getBounds();
    table.setBounds(r.getX(),r.getY()+10,r.getWidth(),r.getHeight()-10);
    
    table.autoSizeAllColumns();
}

int Polytempo_ListComponent::getColumnAutoSizeWidth(int columnId)
{
    int width = getWidth() * 0.25; // 4 columns
    return width;// > 150 ? 150 : width;
}

void Polytempo_ListComponent::backgroundClicked(const MouseEvent& event)
{
    if(event.mods.isPopupMenu())
        showPopupMenu();
    else
        table.deselectAllRows();
}

void Polytempo_ListComponent::mouseDownInRow(int rowNumber, const juce::MouseEvent& event)
{
    if(event.mods.isPopupMenu())
        showPopupMenu();
    else if(event.getNumberOfClicks() > 1)
        table.selectRow(rowNumber);
    else
        table.flipRowSelection(rowNumber);
}

void Polytempo_ListComponent::setSelection(int rowNumber, bool focus)
{
    table.updateContent(); // make sure that the row to be selected exists
    table.selectRow(rowNumber);
    if(focus) focusRow = rowNumber;
}

void Polytempo_ListComponent::paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)
{
    if(rowIsSelected)
        g.fillAll(Colour(235,235,255));

    if(focusRow >= 0)
    {
        EditableTextCustomComponent* cell = ((EditableTextCustomComponent*)table.getCellComponent(1, focusRow));
        if(cell != nullptr) cell->showEditor();
        focusRow = -1;
    }
}
