#pragma once

#include "JuceHeader.h"
#include "Polytempo_ListComponent.h"
#include "../../Data/Polytempo_Sequence.h"


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
    void setSequence();
    Polytempo_Sequence* sequence = nullptr;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_BeatPatternListComponent)
};
