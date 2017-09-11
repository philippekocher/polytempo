/*
  ==============================================================================

    Polytempo_GraphicsAnnotationSettingsDialog.cpp
    Created: 30 Aug 2017 3:32:09pm
    Author:  chris

  ==============================================================================
*/

#include "../../../POLYTEMPO NETWORK/JuceLibraryCode/JuceHeader.h"
#include "Polytempo_GraphicsAnnotationSettingsDialog.h"

//==============================================================================
Polytempo_GraphicsAnnotationSettingsDialog::Polytempo_GraphicsAnnotationSettingsDialog()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
	numRows = 0;
	addAndMakeVisible(table);
	table.setModel(this);
}

Polytempo_GraphicsAnnotationSettingsDialog::~Polytempo_GraphicsAnnotationSettingsDialog()
{
}

void Polytempo_GraphicsAnnotationSettingsDialog::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (Colours::white);
    g.setFont (14.0f);
    g.drawText ("Polytempo_GraphicsAnnotationSettingsDialog", getLocalBounds(),
                Justification::centred, true);   // draw some placeholder text
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

void Polytempo_GraphicsAnnotationSettingsDialog::show()
{
	Polytempo_GraphicsAnnotationSettingsDialog *p = new Polytempo_GraphicsAnnotationSettingsDialog();
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
