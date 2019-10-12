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
    lastColor = Colours::red;
    lastTextSize = STANDARD_FONT_SIZE;
    lastLineWeight = STANDARD_LINE_WEIGHT;
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

	buttonOk.reset(new ImageButton("OK"));
	buttonOk->setImages(false, true, false, imageOk, 1.0f, Colours::transparentWhite, Image(), overOpacity, overColour, Image(), downOpacity, Colours::green);
	buttonOk->setBounds(0, 0, BUTTON_SIZE, BUTTON_SIZE);
	buttonOk->addListener(this);
	pParentComponent->addChildComponent(buttonOk.get());

	buttonCancel.reset(new ImageButton("Cancel"));
	buttonCancel->setImages(false, true, false, imageCancel, 1.0f, Colours::transparentWhite, Image(), overOpacity, overColour, Image(), downOpacity, Colours::red);
	buttonCancel->setBounds(0, 0, BUTTON_SIZE, BUTTON_SIZE);
	buttonCancel->addListener(this);
	pParentComponent->addChildComponent(buttonCancel.get());

	buttonDelete.reset(new ImageButton("Delete"));
	buttonDelete->setImages(false, true, false, imageDelete, 1.0f, Colours::transparentWhite, Image(), overOpacity, overColour, Image(), downOpacity, Colours::red);
	buttonDelete->setBounds(0, 0, BUTTON_SIZE, BUTTON_SIZE);
	buttonDelete->addListener(this);
	pParentComponent->addChildComponent(buttonDelete.get());
    
    colorSelector.reset(new ColourSelector(ColourSelector::showColourspace));
    colorSelector->addChangeListener(this);
    colorSelector->setColour(ColourSelector::backgroundColourId, Colours::white);
    pParentComponent->addChildComponent(colorSelector.get());
    
    transparencyLabel.reset(new ImageComponent());
    transparencyLabel->setImage(CreateImageWithSolidBackground(ImageCache::getFromMemory(BinaryData::transparency_png, BinaryData::transparency_pngSize), BUTTON_SIZE, BUTTON_SIZE));
    pParentComponent->addChildComponent(transparencyLabel.get());
    
    transparencySlider.reset(new Slider());
    transparencySlider->setRange(0, 200, 1);
    transparencySlider->addListener(this);
    transparencySlider->setSliderStyle(Slider::LinearBar);
    pParentComponent->addChildComponent(transparencySlider.get());
    
    textSizeLabel.reset(new ImageComponent());
    textSizeLabel->setImage(CreateImageWithSolidBackground(ImageCache::getFromMemory(BinaryData::textSize_png, BinaryData::textSize_pngSize), BUTTON_SIZE, BUTTON_SIZE));
    pParentComponent->addChildComponent(textSizeLabel.get());
    
    textSizeSlider.reset(new Slider());
    textSizeSlider->setRange(20, 300, 1);
    textSizeSlider->addListener(this);
    textSizeSlider->setSliderStyle(Slider::LinearBar);
    pParentComponent->addChildComponent(textSizeSlider.get());
    
    lineWeightLabel.reset(new ImageComponent());
    lineWeightLabel->setImage(CreateImageWithSolidBackground(ImageCache::getFromMemory(BinaryData::lineWeight_png, BinaryData::lineWeight_pngSize), BUTTON_SIZE, BUTTON_SIZE));
    pParentComponent->addChildComponent(lineWeightLabel.get());
    
    lineWeightSlider.reset(new Slider());
    lineWeightSlider->setRange(1, 50, 0.1);
    lineWeightSlider->addListener(this);
    lineWeightSlider->setSliderStyle(Slider::LinearBar);
    pParentComponent->addChildComponent(lineWeightSlider.get());
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
    
    bool showCancel = show && Polytempo_GraphicsAnnotationManager::getInstance()->getAnnotationMode() != Polytempo_GraphicsAnnotationManager::Edit;
    
	buttonCancel->setVisible(showCancel);
	colorSelector->setVisible(show);
    transparencySlider->setVisible(show);
	textSizeSlider->setVisible(show);
    lineWeightSlider->setVisible(show);
    transparencyLabel->setVisible(show);
    textSizeLabel->setVisible(show);
    lineWeightLabel->setVisible(show);
    
    bool showDelete = show && Polytempo_GraphicsAnnotationManager::getInstance()->getAnchorFlag();
		buttonDelete->setVisible(showDelete);

	pParentComponent->resized();

	if(pAnnotationLayer != nullptr)
		pAnnotationLayer->grabKeyboardFocus();
    
    if(show)
    {
        colorSelector->setCurrentColour(pAnnotationLayer->getTemporaryColor());
        transparencySlider->setValue(255 - pAnnotationLayer->getTemporaryColor().getAlpha());
        textSizeSlider->setValue(pAnnotationLayer->getTemporaryFontSize());
        lineWeightSlider->setValue(pAnnotationLayer->getTemporaryLineWeight());
    }
}

int Polytempo_GraphicsPalette::isVisible() const
{
	return visibleFlag;
}

int Polytempo_GraphicsPalette::resize(Point<int> offset) const
{
	// returns the height of the palette

	int totalWidth = pParentComponent->getWidth()-2;

    int currentY = offset.getY();
    
	int buttonSize = int(totalWidth / 5);
	buttonOk->setBounds(offset.getX(), offset.getY(), buttonSize, buttonSize);
	buttonCancel->setBounds(offset.getX() + buttonSize, offset.getY(), buttonSize, buttonSize);
	buttonDelete->setBounds(offset.getX() + 4 * buttonSize, offset.getY(), buttonSize, buttonSize);

    currentY += buttonSize + 4;
    
    // color selector and sliders
    transparencyLabel->setBounds(offset.getX(), currentY, 30, 30);
    transparencySlider->setBounds(offset.getX() + 32, currentY, totalWidth - 30, 30);
    currentY += 34;
    
    textSizeLabel->setBounds(offset.getX(), currentY, 30, 30);
    textSizeSlider->setBounds(offset.getX() + 32, currentY, totalWidth -30, 30);
    currentY += 34;
    
    lineWeightLabel->setBounds(offset.getX(), currentY, 30, 30);
    lineWeightSlider->setBounds(offset.getX() + 32, currentY, totalWidth - 30, 30);
    currentY += 34;
    
	colorSelector->setBounds(offset.getX(), currentY, totalWidth, 70);
    currentY += 74;
    
    if (isVisible())
		return currentY - offset.getY() + 5;
	
	return 0;
}

Colour Polytempo_GraphicsPalette::getLastColour() const
{
	return lastColor;
}

float Polytempo_GraphicsPalette::getLastTextSize() const
{
    return lastTextSize;
}

float Polytempo_GraphicsPalette::getLastLineWeight() const
{
    return lastLineWeight;
}

void Polytempo_GraphicsPalette::changeListenerCallback(ChangeBroadcaster* source)
{
	if (auto* cs = dynamic_cast<ColourSelector*> (source))
	{
        setTemporaryColor();
	}
}

void Polytempo_GraphicsPalette::buttonStateChanged(Button*)
{
	pAnnotationLayer->stopAutoAccept();
}

void Polytempo_GraphicsPalette::mouseEntered()
{
    pAnnotationLayer->stopAutoAccept();
}

void Polytempo_GraphicsPalette::mouseLeft()
{
    pAnnotationLayer->restartAutoAccept();
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
	
    else if(source == buttonDelete.get())
	{
		pAnnotationLayer->handleDeleteSelected();
	}
}

void Polytempo_GraphicsPalette::setTemporaryColor()
{
    Colour col = colorSelector->getCurrentColour().withAlpha((uint8)(255-transparencySlider->getValue()));
    
    lastColor = col;
    pAnnotationLayer->setTemporaryColor(col);
}

void Polytempo_GraphicsPalette::sliderValueChanged(Slider *slider)
{
    if(slider == transparencySlider.get())
    {
        setTemporaryColor();
    }
    if(slider == textSizeSlider.get())
    {
        setTemporaryFontSize(textSizeSlider->getValue());
        lastTextSize = textSizeSlider->getValue();
    }
    else if(slider == lineWeightSlider.get())
    {
        pAnnotationLayer->setTemporaryLineWeight(lineWeightSlider->getValue());
        lastLineWeight = lineWeightSlider->getValue();
    }
}

void Polytempo_GraphicsPalette::setTemporaryFontSize(float size) const
{
	pAnnotationLayer->setTemporaryFontSize(size);
}
