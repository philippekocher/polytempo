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

//==============================================================================
// This is a custom Label component, which we use for the table's editable text columns.
class EditableTextCustomComponent : public Label
{
public:
	EditableTextCustomComponent(Polytempo_GraphicsAnnotationSettingsDialog& td) : owner(td)
	{
		// double click to edit the label text; single click handled below
		setEditable(false, true, false);
	}

	void mouseDown(const MouseEvent& event) override
	{
		// single click on the label should simply select the row
		owner.getTable()->selectRowsBasedOnModifierKeys(row, event.mods, false);

		Label::mouseDown(event);
	}

	void textWasEdited() override
	{
		bool ok = owner.setText(row, getText());
		if (!ok)
			setText(owner.getText(row), sendNotificationAsync);
	}

	// Our demo code will call this when we may need to update our contents
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
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
	numRows = 0;
	addAndMakeVisible(table);
	table.setModel(this);
	table.getHeader().addColumn("Show", 1, 35);
	table.getHeader().addColumn("Edit", 2, 35);
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

String Polytempo_GraphicsAnnotationSettingsDialog::getText(int row)
{
	return pAnnotationSet->getUnchecked(row)->getAnnotationLayerName();
}

Component* Polytempo_GraphicsAnnotationSettingsDialog::refreshComponentForCell(int rowNumber, int columnId, bool /*isRowSelected*/,
	Component* existingComponentToUpdate)
{
	if (columnId == 1 || columnId == 2)
	{
		jassert(existingComponentToUpdate == nullptr);
		return nullptr;
	}

	// The other columns are editable text columns, for which we use the custom Label component
	EditableTextCustomComponent* textLabel = static_cast<EditableTextCustomComponent*> (existingComponentToUpdate);

	// same as above...
	if (textLabel == nullptr)
		textLabel = new EditableTextCustomComponent(*this);

	textLabel->setRowAndColumn(rowNumber, columnId);
	return textLabel;
}

// This is overloaded from TableListBoxModel, and should fill in the background of the whole row
void Polytempo_GraphicsAnnotationSettingsDialog::paintRowBackground(Graphics& g, int rowNumber, int /*width*/, int /*height*/, bool rowIsSelected)
{
	const Colour alternateColour(getLookAndFeel().findColour(ListBox::backgroundColourId)
		.interpolatedWith(getLookAndFeel().findColour(ListBox::textColourId), 0.03f));
	if (rowIsSelected)
		g.fillAll(Colours::lightblue);
	else if (rowNumber % 2)
		g.fillAll(alternateColour);
}

// This is overloaded from TableListBoxModel, and must paint any cells that aren't using custom
// components.
void Polytempo_GraphicsAnnotationSettingsDialog::paintCell(Graphics& g, int rowNumber, int columnId,
	int width, int height, bool /*rowIsSelected*/)
{
	g.setColour(getLookAndFeel().findColour(ListBox::textColourId));
	//g.setFont(font);

	//if (const XmlElement* rowElement = dataList->getChildElement(rowNumber))
	//{
	//	const String text(rowElement->getStringAttribute(getAttributeNameForColumnId(columnId)));

	//	g.drawText(text, 2, 0, width - 4, height, Justification::centredLeft, true);
	//}

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
