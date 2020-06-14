#pragma once
#include "JuceHeader.h"
#include "Polytempo_ListComponent.h"


class Polytempo_PointListComponent : public Polytempo_ListComponent
{
public:
    Polytempo_PointListComponent();
    ~Polytempo_PointListComponent();

    void paint(Graphics&);
        
    String getText(int rowNumber, int columnID);
    void   setText(String text, int rowNumber, int columnID);
    
    int getNumRows();
    void selectedRowsChanged(int lastRowSelected);
    void paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected);
    Component* refreshComponentForCell (int rowNumber, int columnId, bool isRowSelected, Component* existingComponentToUpdate);
    
    void showPopupMenu();
   
private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_PointListComponent)
};
