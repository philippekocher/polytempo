/*
  ==============================================================================

    Polytempo_GraphicsAnnotationSettingsDialog.h
    Created: 30 Aug 2017 3:32:09pm
    Author:  chris

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "Polytempo_GraphicsAnnotationSet.h"

//==============================================================================
/*
*/
class Polytempo_GraphicsAnnotationSettingsDialog    : public Component, public TableListBoxModel, private ButtonListener
{
public:
	static void show(OwnedArray < Polytempo_GraphicsAnnotationSet>* pAnnotationSet);

	Polytempo_GraphicsAnnotationSettingsDialog(OwnedArray<Polytempo_GraphicsAnnotationSet>* pAnnotationSet);
    ~Polytempo_GraphicsAnnotationSettingsDialog();

    void paint (Graphics&) override;
    void resized() override;
	int getNumRows() override;
	bool setText(const int rowNumber, String newText) const;
	String getText(int row) const;
	bool getShowInfo(int row) const;
	bool setShowInfo(int row, bool state);
	bool getEditInfo(int row) const;
	void setInfoLabel(String str) const;
	void updateInfoLabel() const;
	bool setEditInfo(int row, bool state) const;
	TableListBox* getTable();

	Component* refreshComponentForCell(int rowNumber, int columnId, bool, Component* existingComponentToUpdate) override;
	void paintRowBackground(Graphics& g, int rowNumber, int /*width*/, int /*height*/, bool rowIsSelected) override;
	void paintCell(Graphics& g, int /*rowNumber*/, int /*columnId*/, int width, int height, bool /*rowIsSelected*/) override;
	void buttonClicked(Button*) override;

private:
	int GetNumberOfEditableLayers(int exceptIndex = -1) const;

private:
	TableListBox table;
	ScopedPointer<TextButton> addLayerBtn;
	ScopedPointer<ToggleButton> showAnchorPointsToggle;
	ScopedPointer<Label> infoLabel;
	
	int numRows;
	OwnedArray<Polytempo_GraphicsAnnotationSet>* pAnnotationSet;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_GraphicsAnnotationSettingsDialog)
};
