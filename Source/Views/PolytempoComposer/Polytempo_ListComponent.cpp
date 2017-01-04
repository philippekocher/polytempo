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
    int width = getWidth() * 0.25;
    return width > 150 ? 150 : width;
}

void Polytempo_ListComponent::backgroundClicked(const MouseEvent& event)
{
    if(event.mods.isPopupMenu())
        showPopupMenu();
}

void Polytempo_ListComponent::mouseDownInRow(int rowNumber, const MouseEvent& event)
{
    if(event.mods.isPopupMenu())
        showPopupMenu();
    else
        table.selectRowsBasedOnModifierKeys(rowNumber, event.mods, false);
}

void Polytempo_ListComponent::paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)
{
   // if(rowIsSelected)
//        g.fillAll(Colour(235,235,255));
    if(0 == rowNumber % 2)
        g.fillAll(Colour(235,235,235));
    
//    g.fillRect(0,0,width,height-1);
}