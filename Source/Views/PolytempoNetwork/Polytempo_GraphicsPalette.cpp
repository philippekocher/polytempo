/*
  ==============================================================================

    Polytempo_GraphicsPalette.cpp
    Created: 16 Jul 2018 8:56:42am
    Author:  christian.schweizer

  ==============================================================================
*/

#include "JuceHeader.h"
#include "Polytempo_GraphicsPalette.h"
#include <string>
#include "Polytempo_GraphicsAnnotationManager.h"

//==============================================================================
Polytempo_GraphicsPalette::Polytempo_GraphicsPalette(Polytempo_GraphicsAnnotationLayer* pParent)
{
	this->pParent = pParent;

	Colour overColour = Colour(Colours::purple.getRed(), Colours::purple.getGreen(), Colours::purple.getBlue(), uint8(80));
	float overOpacity = 1.0;
	float downOpacity = 1.0;

	Image imageColorPalette = CreateImageWithSolidBackground(ImageCache::getFromMemory(BinaryData::colorPalette_png, BinaryData::colorPalette_pngSize), BUTTON_SIZE, BUTTON_SIZE);
	Image imageFontSize = CreateImageWithSolidBackground(ImageCache::getFromMemory(BinaryData::fontSize_png, BinaryData::fontSize_pngSize), BUTTON_SIZE, BUTTON_SIZE);
	Image imageOk = CreateImageWithSolidBackground(ImageCache::getFromMemory(BinaryData::yes_png, BinaryData::yes_pngSize), BUTTON_SIZE, BUTTON_SIZE);
	Image imageCancel = CreateImageWithSolidBackground(ImageCache::getFromMemory(BinaryData::no_png, BinaryData::no_pngSize), BUTTON_SIZE, BUTTON_SIZE);
	Image imageSettings = CreateImageWithSolidBackground(ImageCache::getFromMemory(BinaryData::settings_png, BinaryData::settings_pngSize), BUTTON_SIZE, BUTTON_SIZE);
	Image imageDelete = CreateImageWithSolidBackground(ImageCache::getFromMemory(BinaryData::delete_png, BinaryData::delete_pngSize), BUTTON_SIZE, BUTTON_SIZE);

	buttonColor = new ImageButton("Color");
	buttonColor->setImages(false, false, false, imageColorPalette, 1.0f, Colours::transparentWhite, Image(), overOpacity, overColour, Image(), downOpacity, Colours::green);
	buttonColor->setBounds(0, 0, BUTTON_SIZE, BUTTON_SIZE);
	buttonColor->addListener(this);
	pParent->addChildComponent(buttonColor);

	buttonTextSize = new ImageButton("Size");
	buttonTextSize->setImages(false, false, false, imageFontSize, 1.0f, Colours::transparentWhite, Image(), overOpacity, overColour, Image(), downOpacity, Colours::green);
	buttonTextSize->setBounds(0, 0, BUTTON_SIZE, BUTTON_SIZE);
	buttonTextSize->setTriggeredOnMouseDown(true);
	buttonTextSize->addListener(this);
	pParent->addChildComponent(buttonTextSize);

	buttonSettings = new ImageButton("Settings");
	buttonSettings->setImages(false, false, false, imageSettings, 1.0f, Colours::transparentWhite, Image(), overOpacity, overColour, Image(), downOpacity, Colours::green);
	buttonSettings->setBounds(0, 0, BUTTON_SIZE, BUTTON_SIZE);
	buttonSettings->addListener(this);
	pParent->addChildComponent(buttonSettings);

	buttonOk = new ImageButton("OK");
	buttonOk->setImages(false, false, false, imageOk, 1.0f, Colours::transparentWhite, Image(), overOpacity, overColour, Image(), downOpacity, Colours::green);
	buttonOk->setBounds(0, 0, BUTTON_SIZE, BUTTON_SIZE);
	buttonOk->addListener(this);
	pParent->addChildComponent(buttonOk);

	buttonCancel = new ImageButton("Cancel");
	buttonCancel->setImages(false, false, false, imageCancel, 1.0f, Colours::transparentWhite, Image(), overOpacity, overColour, Image(), downOpacity, Colours::red);
	buttonCancel->setBounds(0, 0, BUTTON_SIZE, BUTTON_SIZE);
	buttonCancel->addListener(this);
	pParent->addChildComponent(buttonCancel);

	buttonDelete = new ImageButton("Delete");
	buttonDelete->setImages(false, false, false, imageDelete, 1.0f, Colours::transparentWhite, Image(), overOpacity, overColour, Image(), downOpacity, Colours::red);
	buttonDelete->setBounds(0, 0, BUTTON_SIZE, BUTTON_SIZE);
	buttonDelete->addListener(this);
	pParent->addChildComponent(buttonDelete);

	colorSelector = new ColourSelector(ColourSelector::ColourSelectorOptions::showAlphaChannel | ColourSelector::ColourSelectorOptions::showColourspace | ColourSelector::ColourSelectorOptions::showSliders);
	colorSelector->setBounds(0, 0, 200, 200);
	colorSelector->setCurrentColour(Colours::red);
	colorSelector->addChangeListener(this);
	pParent->addChildComponent(colorSelector);
}

Polytempo_GraphicsPalette::~Polytempo_GraphicsPalette()
{
}


Image Polytempo_GraphicsPalette::CreateImageWithSolidBackground(Image image, int targetWidth, int targetHeight)
{
	Image newImage = Image(Image::RGB, targetWidth, targetHeight, false);
	newImage.clear(newImage.getBounds(), Colours::lightyellow);

	Graphics gFontSize(newImage);
	gFontSize.drawImage(image, 0, 0, targetWidth, targetHeight, 0, 0, image.getWidth(), image.getHeight());

	return newImage;
}

void Polytempo_GraphicsPalette::show(bool show) const
{
	if (show)
	{
		buttonOk->setTopLeftPosition(0, 0);
		buttonCancel->setTopLeftPosition(BUTTON_SIZE, 0);
		buttonColor->setTopLeftPosition(2 * BUTTON_SIZE, 0);
		buttonTextSize->setTopLeftPosition(3 * BUTTON_SIZE, 0);
		buttonSettings->setTopLeftPosition(4 * BUTTON_SIZE, 0);
		buttonDelete->setTopLeftPosition(5 * BUTTON_SIZE, 0);
	}

	buttonOk->setVisible(show);
	buttonCancel->setVisible(show);
	buttonColor->setVisible(show);
	buttonTextSize->setVisible(show);
	buttonSettings->setVisible(show);
	if (Polytempo_GraphicsAnnotationManager::getInstance()->getAnchorFlag())
		buttonDelete->setVisible(show);

	if(!show)
		colorSelector->setVisible(false);

	pParent->grabKeyboardFocus();
}

Colour Polytempo_GraphicsPalette::getCurrentColour() const
{
	return colorSelector->getCurrentColour();
}


void Polytempo_GraphicsPalette::AddFontSizeToMenu(PopupMenu* m, int fontSize) const
{
	m->addItem(fontSize, std::to_string(fontSize), true, pParent->getTemporaryFontSize() == fontSize);
}

PopupMenu Polytempo_GraphicsPalette::getTextSizePopupMenu() const
{
	PopupMenu m;
	AddFontSizeToMenu(&m, 10);
	AddFontSizeToMenu(&m, 20);
	AddFontSizeToMenu(&m, 30);
	AddFontSizeToMenu(&m, 40);
	AddFontSizeToMenu(&m, 50);
	AddFontSizeToMenu(&m, 60);
	AddFontSizeToMenu(&m, 70);
	AddFontSizeToMenu(&m, 80);
	AddFontSizeToMenu(&m, 90);
	AddFontSizeToMenu(&m, 100);

	return m;
}

void Polytempo_GraphicsPalette::changeListenerCallback(ChangeBroadcaster* source)
{
	if (source == colorSelector)
	{
		pParent->setTemporaryColor(colorSelector->getCurrentColour());
	}
}

void Polytempo_GraphicsPalette::buttonClicked(Button* source)
{
	if (source == buttonOk)
	{
		pParent->handleEndEditAccept();
	}
	else if (source == buttonCancel)
	{
		pParent->handleEndEditCancel();
	}
	else if (source == buttonColor)
	{
		pParent->stopAutoAccept();
		colorSelector->setTopLeftPosition(buttonColor->getX(), buttonColor->getY() + buttonColor->getHeight());
		colorSelector->setVisible(!colorSelector->isVisible());
		pParent->setTemporaryColor(colorSelector->getCurrentColour());
	}
	else if (source == buttonTextSize)
	{
		pParent->stopAutoAccept();
		getTextSizePopupMenu().showMenuAsync(PopupMenu::Options().withTargetComponent(buttonTextSize), new FontSizeCallback(this));
	}
	else if (source == buttonSettings)
	{
		pParent->stopAutoAccept();
		Polytempo_GraphicsAnnotationManager::getInstance()->showSettingsDialog();
	}
	else if(source == buttonDelete)
	{
		pParent->handleDeleteSelected();
	}
}

void Polytempo_GraphicsPalette::hitBtnColor()
{
	buttonClicked(buttonColor);
}

void Polytempo_GraphicsPalette::hitBtnTextSize()
{
	buttonClicked(buttonTextSize);
}

void Polytempo_GraphicsPalette::setTemporaryFontSize(float size) const
{
	pParent->setTemporaryFontSize(size);
}
