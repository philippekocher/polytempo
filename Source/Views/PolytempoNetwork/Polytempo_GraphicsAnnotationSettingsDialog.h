/*
  ==============================================================================

    Polytempo_GraphicsAnnotationSettingsDialog.h
    Created: 30 Aug 2017 3:32:09pm
    Author:  chris

  ==============================================================================
*/

#pragma once

#include "../../../POLYTEMPO NETWORK/JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class Polytempo_GraphicsAnnotationSettingsDialog    : public Component, public TableListBoxModel
{
public:
    Polytempo_GraphicsAnnotationSettingsDialog();
    ~Polytempo_GraphicsAnnotationSettingsDialog();

    void paint (Graphics&) override;
    void resized() override;
	int getNumRows() override;

	// This is overloaded from TableListBoxModel, and should fill in the background of the whole row
	void paintRowBackground(Graphics& g, int rowNumber, int /*width*/, int /*height*/, bool rowIsSelected) override;

	// This is overloaded from TableListBoxModel, and must paint any cells that aren't using custom
	// components.
	void paintCell(Graphics& g, int rowNumber, int columnId,
		int width, int height, bool /*rowIsSelected*/) override;
	static void show();

private:
	TableListBox table;
	int numRows;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_GraphicsAnnotationSettingsDialog)
};
