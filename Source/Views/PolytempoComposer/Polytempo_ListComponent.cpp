#include "Polytempo_ListComponent.h"


void Polytempo_ListComponent::resized()
{
    Rectangle<int> r = getBounds();
    table->setBounds(r.getX(),r.getY()+10,r.getWidth(),r.getHeight()-10);
    table->getHeader().resizeAllColumnsToFit(r.getWidth());
}

int Polytempo_ListComponent::getColumnAutoSizeWidth(int /*columnId*/)
{
    int width = int(getWidth() * 0.25); // 4 columns
    return width;
}

void Polytempo_ListComponent::backgroundClicked(const MouseEvent& event)
{
    if(event.mods.isPopupMenu())
        showPopupMenu();
    else
        table->deselectAllRows();
}

void Polytempo_ListComponent::mouseDownInRow(int rowNumber, const juce::MouseEvent& event)
{
    if(event.mods.isPopupMenu())
        showPopupMenu();
    else if(event.getNumberOfClicks() > 1)
        table->selectRow(rowNumber);
    else
        table->flipRowSelection(rowNumber);
}

void Polytempo_ListComponent::setSelection(int rowNumber, bool focus)
{
    table->updateContent(); // make sure that the row to be selected exists
    table->selectRow(rowNumber);
    if(focus) focusRow = rowNumber;
}

void Polytempo_ListComponent::paintRowBackground(Graphics& g, int /*rowNumber*/, int /*width*/, int /*height*/, bool rowIsSelected)
{
    if(rowIsSelected)
        g.fillAll(Colour(235,235,255));

    if(focusRow >= 0)
    {
        MessageManager::callAsync([this]() {
            EditableTextCustomComponent* cell = ((EditableTextCustomComponent*)table->getCellComponent(1, focusRow));
            if(cell != nullptr) cell->showEditor();
            focusRow = -1;
        });
    }
}
