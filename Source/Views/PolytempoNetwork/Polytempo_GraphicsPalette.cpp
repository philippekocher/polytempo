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

juce_ImplementSingleton(Polytempo_GraphicsPalette)

//==============================================================================
Polytempo_GraphicsPalette::Polytempo_GraphicsPalette(): pAnnotationLayer(nullptr), visibleFlag(false), pParentComponent(nullptr)
{
}

Polytempo_GraphicsPalette::~Polytempo_GraphicsPalette()
{
}

void Polytempo_GraphicsPalette::initialize(Component* pParent)
{
	pParentComponent = pParent;

	Colour overColour = Colour(Colours::purple.getRed(), Colours::purple.getGreen(), Colours::purple.getBlue(), uint8(80));
	float overOpacity = 1.0;
	float downOpacity = 1.0;

	Image imageColorPalette = CreateImageWithSolidBackground(ImageCache::getFromMemory(BinaryData::colorPalette_png, BinaryData::colorPalette_pngSize), BUTTON_SIZE, BUTTON_SIZE);
	Image imageFontSize = CreateImageWithSolidBackground(ImageCache::getFromMemory(BinaryData::fontSize_png, BinaryData::fontSize_pngSize), BUTTON_SIZE, BUTTON_SIZE);
	Image imageOk = CreateImageWithSolidBackground(ImageCache::getFromMemory(BinaryData::yes_png, BinaryData::yes_pngSize), BUTTON_SIZE, BUTTON_SIZE);
	Image imageCancel = CreateImageWithSolidBackground(ImageCache::getFromMemory(BinaryData::no_png, BinaryData::no_pngSize), BUTTON_SIZE, BUTTON_SIZE);
	Image imageDelete = CreateImageWithSolidBackground(ImageCache::getFromMemory(BinaryData::delete_png, BinaryData::delete_pngSize), BUTTON_SIZE, BUTTON_SIZE);

	buttonColor.reset(new ImageButton("Color"));
	buttonColor->setImages(false, false, false, imageColorPalette, 1.0f, Colours::transparentWhite, Image(), overOpacity, overColour, Image(), downOpacity, Colours::green);
	buttonColor->setBounds(0, 0, BUTTON_SIZE, BUTTON_SIZE);
	buttonColor->addListener(this);
	pParentComponent->addChildComponent(buttonColor.get());

	buttonTextSize.reset(new ImageButton("Size"));
	buttonTextSize->setImages(false, false, false, imageFontSize, 1.0f, Colours::transparentWhite, Image(), overOpacity, overColour, Image(), downOpacity, Colours::green);
	buttonTextSize->setBounds(0, 0, BUTTON_SIZE, BUTTON_SIZE);
	buttonTextSize->setTriggeredOnMouseDown(true);
	buttonTextSize->addListener(this);
	pParentComponent->addChildComponent(buttonTextSize.get());

	buttonOk.reset(new ImageButton("OK"));
	buttonOk->setImages(false, false, false, imageOk, 1.0f, Colours::transparentWhite, Image(), overOpacity, overColour, Image(), downOpacity, Colours::green);
	buttonOk->setBounds(0, 0, BUTTON_SIZE, BUTTON_SIZE);
	buttonOk->addListener(this);
	pParentComponent->addChildComponent(buttonOk.get());

	buttonCancel.reset(new ImageButton("Cancel"));
	buttonCancel->setImages(false, false, false, imageCancel, 1.0f, Colours::transparentWhite, Image(), overOpacity, overColour, Image(), downOpacity, Colours::red);
	buttonCancel->setBounds(0, 0, BUTTON_SIZE, BUTTON_SIZE);
	buttonCancel->addListener(this);
	pParentComponent->addChildComponent(buttonCancel.get());

	buttonDelete.reset(new ImageButton("Delete"));
	buttonDelete->setImages(false, false, false, imageDelete, 1.0f, Colours::transparentWhite, Image(), overOpacity, overColour, Image(), downOpacity, Colours::red);
	buttonDelete->setBounds(0, 0, BUTTON_SIZE, BUTTON_SIZE);
	buttonDelete->addListener(this);
	pParentComponent->addChildComponent(buttonDelete.get());

	colorSelector.reset(new ColourSelector(ColourSelector::ColourSelectorOptions::showAlphaChannel | ColourSelector::ColourSelectorOptions::showColourspace | ColourSelector::ColourSelectorOptions::showSliders | ColourSelector::ColourSelectorOptions::showColourAtTop));
	colorSelector->setBounds(0, 0, 200, 200);
	colorSelector->setCurrentColour(Colours::red);
	colorSelector->addChangeListener(this);
	pParentComponent->addChildComponent(colorSelector.get());
}


Image Polytempo_GraphicsPalette::CreateImageWithSolidBackground(Image image, int targetWidth, int targetHeight)
{
	Image newImage = Image(Image::RGB, targetWidth, targetHeight, false);
	newImage.clear(newImage.getBounds(), Colours::white);

	Graphics gFontSize(newImage);
	gFontSize.drawImage(image, 0, 0, targetWidth, targetHeight, 0, 0, image.getWidth(), image.getHeight());

	return newImage;
}

void Polytempo_GraphicsPalette::setAnnotationLayer(Polytempo_GraphicsAnnotationLayer* pParent)
{
	this->pAnnotationLayer = pParent;
}

void Polytempo_GraphicsPalette::setVisible(bool show)
{
	visibleFlag = show;

	buttonOk->setVisible(show);
	buttonCancel->setVisible(show);
	buttonColor->setVisible(show);
	buttonTextSize->setVisible(show);
	if (Polytempo_GraphicsAnnotationManager::getInstance()->getAnchorFlag())
		buttonDelete->setVisible(show);

	if (!show)
		colorSelector->setVisible(false);

	pParentComponent->resized();

	if(pAnnotationLayer != nullptr)
		pAnnotationLayer->grabKeyboardFocus();
}

int Polytempo_GraphicsPalette::isVisible() const
{
	return visibleFlag;
}

void Polytempo_GraphicsPalette::resize(Point<int> offset) const
{
	buttonOk->setTopLeftPosition(offset.getX(), offset.getY());
	buttonCancel->setTopLeftPosition(offset.getX() + BUTTON_SIZE, offset.getY());
	buttonColor->setTopLeftPosition(offset.getX() + 2 * BUTTON_SIZE, offset.getY());
	buttonTextSize->setTopLeftPosition(offset.getX() + 3 * BUTTON_SIZE, offset.getY());
	buttonDelete->setTopLeftPosition(offset.getX() + 4 * BUTTON_SIZE, offset.getY());
}

Colour Polytempo_GraphicsPalette::getCurrentColour() const
{
	return colorSelector->getCurrentColour();
}


void Polytempo_GraphicsPalette::AddFontSizeToMenu(PopupMenu* m, int fontSize) const
{
	m->addItem(fontSize, std::to_string(fontSize), true, pAnnotationLayer->getTemporaryFontSize() == fontSize);
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
	if (source == colorSelector.get())
	{
		pAnnotationLayer->setTemporaryColor(colorSelector->getCurrentColour());
	}
}

void Polytempo_GraphicsPalette::buttonStateChanged(Button*)
{
	pAnnotationLayer->stopAutoAccept();
}

void Polytempo_GraphicsPalette::buttonClicked(Button* source)
{
	if (source == buttonOk.get())
	{
		pAnnotationLayer->handleEndEditAccept();
	}
	else if (source == buttonCancel.get())
	{
		pAnnotationLayer->handleEndEditCancel();
	}
	else if (source == buttonColor.get())
	{
		pAnnotationLayer->stopAutoAccept();
		colorSelector->setTopLeftPosition(5, buttonOk->getBottom());
		colorSelector->setVisible(!colorSelector->isVisible());
		colorSelector->setCurrentColour(pAnnotationLayer->getTemporaryColor());
	}
	else if (source == buttonTextSize.get())
	{
		pAnnotationLayer->stopAutoAccept();
		getTextSizePopupMenu().showMenuAsync(PopupMenu::Options().withTargetComponent(buttonTextSize.get()), new FontSizeCallback(this));
	}
	else if(source == buttonDelete.get())
	{
		pAnnotationLayer->handleDeleteSelected();
	}
}

void Polytempo_GraphicsPalette::hitBtnColor()
{
	buttonClicked(buttonColor.get());
}

void Polytempo_GraphicsPalette::hitBtnTextSize()
{
	buttonClicked(buttonTextSize.get());
}

void Polytempo_GraphicsPalette::setTemporaryFontSize(float size) const
{
	pAnnotationLayer->setTemporaryFontSize(size);
}
