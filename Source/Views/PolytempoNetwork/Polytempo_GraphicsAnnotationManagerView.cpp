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

void Polytempo_GraphicsAnnotationManagerView::paint (Graphics&)
{
}

void Polytempo_GraphicsAnnotationManagerView::resized()
{
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
		Polytempo_GraphicsAnnotationManager::getInstance()->showSettingsDialog();
	}
}
