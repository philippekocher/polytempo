/*
  ==============================================================================

    Polytempo_GraphicsAnnotationSettingsDialog.cpp
    Created: 30 Aug 2017 3:32:09pm
    Author:  chris

  ==============================================================================
*/

#include "JuceHeader.h"
#include "Polytempo_GraphicsAnnotationSettingsDialog.h"
#include "Polytempo_GraphicsAnnotationSet.h"

#define COLUMN_ID_SHOW 1
#define COLUMN_ID_EDIT 2


class ToggleColumnCustomComponent : public Component,
	private ButtonListener
{
public:
	ToggleColumnCustomComponent(Polytempo_GraphicsAnnotationSettingsDialog& td) : owner(td)
	{
		addAndMakeVisible(checkBox);
		checkBox.addListener(this);
		checkBox.setWantsKeyboardFocus(false);
	}

	void resized() override
	{
		checkBox.setBoundsInset(BorderSize<int>(2));
	}

	void setRowAndColumn(int newRow, int newColumn)
	{
		row = newRow;
		columnId = newColumn;
		if (columnId == COLUMN_ID_SHOW)
			checkBox.setToggleState(owner.getShowInfo(row), dontSendNotification);
		else if (columnId == COLUMN_ID_EDIT)
			checkBox.setToggleState(owner.getEditInfo(row), dontSendNotification);

		checkBox.repaint();
	}

	void buttonClicked(Button*) override
	{
		if(columnId == COLUMN_ID_SHOW)
			owner.setShowInfo(row, checkBox.getToggleState());
		else if(columnId == COLUMN_ID_EDIT)
			owner.setEditInfo(row, checkBox.getToggleState());

		setRowAndColumn(row, columnId);
	}

private:
	Polytempo_GraphicsAnnotationSettingsDialog& owner;
	ToggleButton checkBox;
	int row, columnId;
};

class EditableTextCustomComponent : public Label
{
public:
	EditableTextCustomComponent(Polytempo_GraphicsAnnotationSettingsDialog& td) : owner(td)
	{
		setEditable(false, true, false);
	}

	void mouseDown(const MouseEvent& event) override
	{
		owner.getTable()->selectRowsBasedOnModifierKeys(row, event.mods, false);
		Label::mouseDown(event);
	}

	void textWasEdited() override
	{
		bool ok = owner.setText(row, getText());
		if (!ok)
			setText(owner.getText(row), sendNotificationAsync);
	}

	void setRowAndColumn(const int newRow, const int newColumn)
	{
		row = newRow;
		columnId = newColumn;
		setText(owner.getText(row), dontSendNotification);
	}

	void paint(Graphics& g) override
	{
		auto& lf = getLookAndFeel();
		if (!dynamic_cast<LookAndFeel_V4*> (&lf))
			lf.setColour(textColourId, Colours::black);

		Label::paint(g);
	}

private:
	Polytempo_GraphicsAnnotationSettingsDialog& owner;
	int row, columnId;
	Colour textColour;
};

//==============================================================================
Polytempo_GraphicsAnnotationSettingsDialog::Polytempo_GraphicsAnnotationSettingsDialog(OwnedArray < Polytempo_GraphicsAnnotationSet>* pAnnotationSet)
{
    addAndMakeVisible(table);
	table.setModel(this);
	table.getHeader().addColumn("Show", COLUMN_ID_SHOW, 35);
	table.getHeader().addColumn("Edit", COLUMN_ID_EDIT, 35);
	table.getHeader().addColumn("Name", 3, 200);
	table.getHeader().resizeAllColumnsToFit(getWidth());
	numRows = pAnnotationSet->size();
	this->pAnnotationSet = pAnnotationSet;
}

Polytempo_GraphicsAnnotationSettingsDialog::~Polytempo_GraphicsAnnotationSettingsDialog()
{
}

void Polytempo_GraphicsAnnotationSettingsDialog::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
}

void Polytempo_GraphicsAnnotationSettingsDialog::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
	
	// position our table with a gap around its edge
	table.setBoundsInset(BorderSize<int>(8));
}

int Polytempo_GraphicsAnnotationSettingsDialog::getNumRows()
{
	return numRows;
}

bool Polytempo_GraphicsAnnotationSettingsDialog::setText(const int rowNumber, String newText) const
{
	bool ok = pAnnotationSet->getUnchecked(rowNumber)->setAnnotationLayerName(newText);
	if (!ok)
	{
		AlertWindow::showMessageBox(AlertWindow::WarningIcon, "Layer renaming", "Renaming layer failed");
		return false;
	}

	return true;
}

String Polytempo_GraphicsAnnotationSettingsDialog::getText(int row) const
{
	return pAnnotationSet->getUnchecked(row)->getAnnotationLayerName();
}

bool Polytempo_GraphicsAnnotationSettingsDialog::getShowInfo(int row) const
{
	return pAnnotationSet->getUnchecked(row)->getShow();
}

bool Polytempo_GraphicsAnnotationSettingsDialog::setShowInfo(int row, bool state)
{
	bool ok = pAnnotationSet->getUnchecked(row)->setShow(state);
	if (!ok)
	{
		AlertWindow::showMessageBox(AlertWindow::WarningIcon, "Set visibility state", "Setting visibility state failed");
		return false;
	}

	return true;
}

bool Polytempo_GraphicsAnnotationSettingsDialog::getEditInfo(int row) const
{
	return pAnnotationSet->getUnchecked(row)->getEdit();
}

int Polytempo_GraphicsAnnotationSettingsDialog::GetNumberOfEditableLayers(int exceptIndex) const
{
	int count = 0;
	for(int i = 0; i < pAnnotationSet->size(); i++)
	{
		if(i != exceptIndex && pAnnotationSet->getUnchecked(i)->getEdit())
		{
			count++;
		}
	}

	return count;
}

bool Polytempo_GraphicsAnnotationSettingsDialog::setEditInfo(int row, bool state) const
{
	int numberOfEditableLayers = GetNumberOfEditableLayers(row);
	if(numberOfEditableLayers == 0 && state == false)
	{
		AlertWindow::showMessageBox(AlertWindow::WarningIcon, "Set edit state", "Setting edit state failed, at least one layer must be editable!");
		return false;
	}
	if(numberOfEditableLayers + (state ? 1 : 0) > 1)
	{
		//Todo	warning
	}

	bool ok = pAnnotationSet->getUnchecked(row)->setEdit(state);
	if (!ok)
	{
		AlertWindow::showMessageBox(AlertWindow::WarningIcon, "Set edit state", "Setting edit state failed");
		return false;
	}

	return true;
}

Component* Polytempo_GraphicsAnnotationSettingsDialog::refreshComponentForCell(int rowNumber, int columnId, bool /*isRowSelected*/,
	Component* existingComponentToUpdate)
{
	if (columnId == COLUMN_ID_SHOW || columnId == COLUMN_ID_EDIT)
	{
		ToggleColumnCustomComponent* toggleComponent = static_cast<ToggleColumnCustomComponent*> (existingComponentToUpdate);
		if (toggleComponent == nullptr)
			toggleComponent = new ToggleColumnCustomComponent(*this);

		toggleComponent->setRowAndColumn(rowNumber, columnId);
		return toggleComponent;
	}

	EditableTextCustomComponent* textLabel = static_cast<EditableTextCustomComponent*> (existingComponentToUpdate);
	if (textLabel == nullptr)
		textLabel = new EditableTextCustomComponent(*this);

	textLabel->setRowAndColumn(rowNumber, columnId);
	return textLabel;
}

void Polytempo_GraphicsAnnotationSettingsDialog::paintRowBackground(Graphics& g, int rowNumber, int /*width*/, int /*height*/, bool rowIsSelected)
{
	const Colour alternateColour(getLookAndFeel().findColour(ListBox::backgroundColourId)
		.interpolatedWith(getLookAndFeel().findColour(ListBox::textColourId), 0.03f));
	if (rowIsSelected)
		g.fillAll(Colours::lightblue);
	else if (rowNumber % 2)
		g.fillAll(alternateColour);
}

void Polytempo_GraphicsAnnotationSettingsDialog::paintCell(Graphics& g, int /*rowNumber*/, int /*columnId*/,
	int width, int height, bool /*rowIsSelected*/)
{
	g.setColour(getLookAndFeel().findColour(ListBox::textColourId));
	g.setColour(getLookAndFeel().findColour(ListBox::backgroundColourId));
	g.fillRect(width - 1, 0, 1, height);
}

void Polytempo_GraphicsAnnotationSettingsDialog::show(OwnedArray < Polytempo_GraphicsAnnotationSet>* pAnnotationSet)
{
	Polytempo_GraphicsAnnotationSettingsDialog *p = new Polytempo_GraphicsAnnotationSettingsDialog(pAnnotationSet);
	p->setSize(500, 690);

	DialogWindow::LaunchOptions options;
	options.content.setOwned(p);
	options.dialogTitle = "Annotation settings";
	options.dialogBackgroundColour = Colours::white;
	options.escapeKeyTriggersCloseButton = true;
	options.useNativeTitleBar = true;
	options.resizable = false;

	options.launchAsync();
}

TableListBox* Polytempo_GraphicsAnnotationSettingsDialog::getTable()
{
	return &table;
}
