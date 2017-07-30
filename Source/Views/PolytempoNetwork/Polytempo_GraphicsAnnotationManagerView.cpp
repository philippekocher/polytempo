/*
  ==============================================================================

    Polytempo_GraphicsAnnotationManagerView.cpp
    Created: 28 Jul 2017 4:27:03pm
    Author:  chris

  ==============================================================================
*/

#include "../../../POLYTEMPO NETWORK/JuceLibraryCode/JuceHeader.h"
#include "Polytempo_GraphicsAnnotationManagerView.h"
#include "Polytempo_GraphicsAnnotationManager.h"

//==============================================================================
Polytempo_GraphicsAnnotationManagerView::Polytempo_GraphicsAnnotationManagerView()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

	addAndMakeVisible(btnEdit = new TextButton("Edit"));
	btnEdit->addListener(this);

	addAndMakeVisible(btnLoad = new TextButton("Load"));
	btnLoad->addListener(this);

	addAndMakeVisible(btnSave = new TextButton("Save"));
	btnSave->addListener(this);
}

Polytempo_GraphicsAnnotationManagerView::~Polytempo_GraphicsAnnotationManagerView()
{
}

void Polytempo_GraphicsAnnotationManagerView::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
	
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (Colours::white);
    g.setFont (14.0f);
    g.drawText ("Annotations", getLocalBounds(),
                Justification::centred, true);   // draw some placeholder text
	*/
}

void Polytempo_GraphicsAnnotationManagerView::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

	//annotationList->setBounds(getBounds());

	btnLoad->setBounds(0, 0, getLocalBounds().getWidth(), getLocalBounds().getHeight() / 4);
	btnSave->setBounds(0, btnLoad->getBottom(), getLocalBounds().getWidth(), getLocalBounds().getHeight() / 4);
	btnEdit->setBounds(0, btnSave->getBottom(), getWidth(), getHeight() - btnSave->getBottom());
}

void Polytempo_GraphicsAnnotationManagerView::buttonClicked(Button* source)
{
	if(source == btnSave)
	{
		Polytempo_GraphicsAnnotationManager::getInstance()->saveAll();
	}
	else if(source == btnLoad)
	{
		Polytempo_GraphicsAnnotationManager::getInstance()->initialize("C:\\temp", "Viola");	// todo: remove
	}
	else if(source == btnEdit)
	{
		// Todo
	}
}
