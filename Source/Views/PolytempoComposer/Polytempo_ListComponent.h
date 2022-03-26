#pragma once

#include "JuceHeader.h"


class Polytempo_TableListBox : public TableListBox
{
public:
    bool keyPressed(const KeyPress& key)
    {
        // keys that should be ingnored by the table
        if(key.getModifiers() == ModifierKeys::commandModifier) return false;
        if(key == KeyPress::returnKey) return false;
        
        // default behaviour for other keys
        return ListBox::keyPressed(key);
    }
};

class Polytempo_ListComponent : public Component,
                                public TableListBoxModel
{
public:
    Polytempo_ListComponent() { setFocusContainerType(FocusContainerType::keyboardFocusContainer); }
    //~Polytempo_ListComponent();
    
    void resized();
    int getColumnAutoSizeWidth (int columnId);
    
    void backgroundClicked(const MouseEvent&);
    void mouseDownInRow(int rowNumber, const MouseEvent&);
    void setSelection(int rowNumber, bool focus = true);
    
    void paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected);
    
    virtual void setText(String text, int rowNumber, int columnID) = 0;
    virtual void showPopupMenu() = 0;
    
protected:
    Polytempo_TableListBox table;
    int focusRow = -1;
    
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
    }
    
    ~EditableTextCustomComponent() {}
    
    void mouseDown(const MouseEvent& event)
    {
        owner->mouseDownInRow(rowNumber, event);
    }
    
    void mouseDoubleClick(const MouseEvent& /*event*/)
    {
        if(isEditable()) showEditor();
    }
    
    void textWasEdited()
    {
        owner->setText(getText(), rowNumber, columnId);
    }
    
    void setRowAndColumn(const int newRow, const int newColumn)
    {
        rowNumber = newRow;
        columnId  = newColumn;
    }
    
    void focusGained(FocusChangeType cause)
    {
        if(!isBeingEdited() &&
           !isCurrentlyModal() &&
           cause == focusChangedByTabKey)
            showEditor();
    }
    
private:
    Polytempo_ListComponent* owner;
    int rowNumber, columnId;
};

class CheckboxTableCell : public Component,  public Button::Listener
{
public:
    CheckboxTableCell (Polytempo_ListComponent* td)  : owner (td)
    {
        addAndMakeVisible (toggle);
        toggle.addListener(this);
    }

    void resized() override
    {
        Rectangle<int> r = getParentComponent()->getBounds();
        toggle.setBounds(10,0,20,r.getHeight());
    }

    void buttonClicked (Button* button) override
    {
        owner->setText(String((int)button->getToggleState()), rowNumber, columnId);
    }
    
    void setRowAndColumn (int newRow, int newColumn)
    {
        rowNumber = newRow;
        columnId  = newColumn;
    }
    
    void setState(bool state)
    {
        toggle.setToggleState(state, dontSendNotification);
    }

private:
    Polytempo_ListComponent* owner;
    ToggleButton toggle;
    int rowNumber, columnId;
};
