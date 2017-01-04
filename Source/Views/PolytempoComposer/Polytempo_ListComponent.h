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

#ifndef __Polytempo_ListComponent__
#define __Polytempo_ListComponent__


#include "../JuceLibraryCode/JuceHeader.h"


class Polytempo_ListComponent : public Component,
                                public TableListBoxModel
{
public:
    Polytempo_ListComponent() {}
    //~Polytempo_ListComponent();
    
    //void paint(Graphics&);
    void resized();
    int getColumnAutoSizeWidth (int columnId);
    
    void backgroundClicked(const MouseEvent&);
    void mouseDownInRow(int rowNumber, const MouseEvent&);
    
    void paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected);
    
    virtual void setText(String text, int rowNumber, int columnID) = 0;
    virtual void showPopupMenu() = 0;
    
protected:
    TableListBox table;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_ListComponent)
};


class EditableTextCustomComponent : public Label
{
public:
    EditableTextCustomComponent(Polytempo_ListComponent* td) : owner (td)
    {
        setEditable(false, true, false); // double click to edit the label text
        setColour(textWhenEditingColourId, Colours::black);
        setColour(TextEditor::highlightedTextColourId, Colours::black);
        setColour(TextEditor::highlightColourId, Colours::lightblue);
        setColour(TextEditor::focusedOutlineColourId, Colours::lightblue);
        
        createFocusTraverser();
    }
    
    ~EditableTextCustomComponent() {}
    
    void mouseDown (const MouseEvent& event)
    {
        // single click on the label should select the row
        owner->mouseDownInRow(rowNumber, event);
    }
    
    void editorShown(TextEditor* editor)
    {
        Point<int> point = editor->getMouseXYRelative();
        int index = editor->getTextIndexAt(point.x, point.y);
        editor->setHighlightedRegion(Range<int>::emptyRange(index));
    }
   
    void textWasEdited()
    {
        owner->setText(getText(), rowNumber, columnId);
    }
    
    void setRowAndColumn (const int newRow, const int newColumn)
    {
        rowNumber = newRow;
        columnId  = newColumn;
    }
    
    void focusGained(FocusChangeType cause)
    {
        if(!isBeingEdited() && !isCurrentlyModal()) showEditor();
    }
    
private:
    Polytempo_ListComponent* owner;
    int rowNumber, columnId;
};







#endif  // __Polytempo_ListComponent__
