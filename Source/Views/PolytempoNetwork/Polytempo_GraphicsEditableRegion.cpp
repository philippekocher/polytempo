/*
  ==============================================================================

    Polytempo_GraphicsEditableRegion.cpp
    Created: 27 Jul 2017 8:40:18am
    Author:  chris

  ==============================================================================
*/

#include "../../../POLYTEMPO NETWORK/JuceLibraryCode/JuceHeader.h"
#include "Polytempo_GraphicsEditableRegion.h"
#include "Polytempo_GraphicsAnnotationManager.h"
#include <string>

//==============================================================================
Polytempo_GraphicsEditableRegion::Polytempo_GraphicsEditableRegion()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
	status = Default;
	
	int width = 32;
	int height = 32;
	Colour overColour = Colour(Colours::purple.getRed(), Colours::purple.getGreen(), Colours::purple.getBlue(), uint8(80));
	float overOpacity = 1.0;
	float downOpacity = 1.0;

	Image imageColorPalette = ImageCache::getFromMemory(BinaryData::colorPalette_png, BinaryData::colorPalette_pngSize);
	Image imageFontSize = ImageCache::getFromMemory(BinaryData::fontSize_png, BinaryData::fontSize_pngSize);
	Image imageOk = ImageCache::getFromMemory(BinaryData::yes_png, BinaryData::yes_pngSize);
	Image imageCancel = ImageCache::getFromMemory(BinaryData::no_png, BinaryData::no_pngSize);
	Image imageSettings = ImageCache::getFromMemory(BinaryData::settings_png, BinaryData::settings_pngSize);

	buttonColor = new ImageButton("Color");
	buttonColor->setImages(false, false, false, imageColorPalette, 1.0f, Colours::transparentWhite, Image::null, overOpacity, overColour, Image::null, downOpacity, Colours::green);
	buttonColor->setBounds(0, 0, width, height);
	buttonColor->addListener(this);
	addChildComponent(buttonColor);

	buttonTextSize = new ImageButton("Size");
	buttonTextSize->setImages(false, false, false, imageFontSize, 1.0f, Colours::transparentWhite, Image::null, overOpacity, overColour, Image::null, downOpacity, Colours::green);
	buttonTextSize->setBounds(0, 0, width, height);
	buttonTextSize->setTriggeredOnMouseDown(true);
	buttonTextSize->addListener(this);
	addChildComponent(buttonTextSize);

	buttonSettings = new ImageButton("Settings");
	buttonSettings->setImages(false, false, false, imageSettings, 1.0f, Colours::transparentWhite, Image::null, overOpacity, overColour, Image::null, downOpacity, Colours::green);
	buttonSettings->setBounds(0, 0, width, height);
	buttonSettings->addListener(this);
	addChildComponent(buttonSettings);

	buttonOk = new ImageButton("OK");
	buttonOk->setImages(false, false, false, imageOk, 1.0f, Colours::transparentWhite, Image::null, overOpacity, overColour, Image::null, downOpacity, Colours::green);
	buttonOk->setBounds(0, 0, width, height);
	buttonOk->addListener(this);
	addChildComponent(buttonOk);
	
	buttonCancel = new ImageButton("Cancel");
	buttonCancel->setImages(false, false, false, imageCancel, 1.0f, Colours::transparentWhite, Image::null, overOpacity, overColour, Image::null, downOpacity, Colours::red);
	buttonCancel->setBounds(0, 0, width, height);
	buttonCancel->addListener(this);
	addChildComponent(buttonCancel);

	colorSelector = new ColourSelector(ColourSelector::ColourSelectorOptions::showAlphaChannel | ColourSelector::ColourSelectorOptions::showColourspace | ColourSelector::ColourSelectorOptions::showSliders);
	colorSelector->setBounds(0, 0, 200, 200);
	colorSelector->setCurrentColour(Colours::red);
	colorSelector->addChangeListener(this);
	addChildComponent(colorSelector);

	addKeyListener(this);

	Polytempo_GraphicsAnnotationManager::getInstance()->addChangeListener(this);

	startTimer(MIN_INTERVAL_BETWEEN_REPAINTS_MS);
}

Polytempo_GraphicsEditableRegion::~Polytempo_GraphicsEditableRegion()
{
	stopTimer();
}

void Polytempo_GraphicsEditableRegion::paintAnnotation(Graphics& g, const Polytempo_GraphicsAnnotation* annotation)
{
	PathStrokeType strokeType(PathStrokeType::rounded);

	g.setColour(annotation->color);
	if (!annotation->freeHandPath.isEmpty())
		g.strokePath(annotation->freeHandPath, strokeType);
	
	if (!annotation->text.isEmpty())
	{
		g.setFont(annotation->fontSize);
		g.drawSingleLineText(annotation->text, int(annotation->referencePoint.getX()) + 10, int(annotation->referencePoint.getY()));
	}

	if(Polytempo_GraphicsAnnotationManager::getInstance()->getAnchorFlag())
	{
		g.setColour(Colours::blue);
		g.fillRoundedRectangle(annotation->referencePoint.getX() - 8.0f, annotation->referencePoint.getY() - 8.0f, 16.0f, 16.0f, 4.0f);
	}
}

void Polytempo_GraphicsEditableRegion::paint (Graphics& g)
{
	paintContent(g);
	
	g.addTransform(imageToScreen);

	if (status == FreehandEditing)
	{
		g.drawArrow(Line<float>(temporaryAnnotation.referencePoint.translated(0.0f, buttonsAboveReferencePoint ? -50.0f : 50.0f), temporaryAnnotation.referencePoint).toFloat(), 4.0f, 8.0f, 8.0f);

		paintAnnotation(g, &temporaryAnnotation);
	}

	for (Polytempo_GraphicsAnnotation* annotation : annotations)
	{
		paintAnnotation(g, annotation);
	}
}

void Polytempo_GraphicsEditableRegion::resized()
{
	resizeContent();
	screenToImage = AffineTransform::translation(-float(getX()), -float(getY()));
	screenToImage = screenToImage.scale(currentImageRectangle.getWidth() / float(targetArea.getWidth()), currentImageRectangle.getHeight() / float(targetArea.getHeight()));
	screenToImage = screenToImage.translated(currentImageRectangle.getX(), currentImageRectangle.getY() - targetArea.getY() / float(targetArea.getHeight()) * currentImageRectangle.getHeight());

	imageToScreen = screenToImage.inverted();
}

void Polytempo_GraphicsEditableRegion::setImage(Image* img, var rect, String imageId)
{
	Array<var> r = *rect.getArray();
	float relTopLeftX = float(r[0]);
	float relTopLeftY = float(r[1]);
	float relWidth = float(r[2]);
	float relHeight = float(r[3]);
	float imgWidth = float(img->getWidth());
	float imgHeight = float(img->getHeight());
	currentImageRectangle = Rectangle<float>(
		imgWidth * relTopLeftX, 
		imgHeight * relTopLeftY, 
		imgWidth * relWidth, 
		imgHeight * relHeight);

	currentImageId = imageId;

	setViewImage(img, rect);

	Polytempo_GraphicsAnnotationManager::getInstance()->getAnnotationsForImage(currentImageId, &annotations);

	resized();
}

void Polytempo_GraphicsEditableRegion::mouseUp(const MouseEvent& e)
{
	if(status == Default)
	{
		if(e.getLengthOfMousePress() > MIN_MOUSE_DOWN_TIME_MS)
		{
			handleStartEditing(e.getPosition());
		}
	}
}

void Polytempo_GraphicsEditableRegion::mouseDown(const MouseEvent& e)
{
	if (status == Default)
	{
		if (e.source.isTouch())
		{
			handleStartEditing(e.getPosition());
		}
	}
	if(status == FreehandEditing)
	{
		float x = float(e.getPosition().getX());
		float y = float(e.getPosition().getY());
		screenToImage.transformPoint(x, y);

		temporaryAnnotation.freeHandPath.startNewSubPath(Point<float>(x, y));
	}
}

void Polytempo_GraphicsEditableRegion::handleFreeHandPainting(const Point<int>& mousePosition)
{
	if (temporaryAnnotation.freeHandPath.isEmpty() || lastPathPoint.getDistanceFrom(mousePosition) > 5)
	{
		float x = float(mousePosition.getX());
		float y = float(mousePosition.getY());
		screenToImage.transformPoint(x, y);

		if (temporaryAnnotation.freeHandPath.isEmpty())
		{
			temporaryAnnotation.freeHandPath.addLineSegment(Line<float>(x, y, x, y), FREE_HAND_LINE_THICKNESS);
		}
		else
		{
			temporaryAnnotation.freeHandPath.lineTo(x, y);
		}

		lastPathPoint = mousePosition;
		repaintRequired = true;
	}
}

void Polytempo_GraphicsEditableRegion::mouseDrag(const MouseEvent& e)
{
	if(status == FreehandEditing)
	{
		handleFreeHandPainting(e.getPosition());
	}
}

void Polytempo_GraphicsEditableRegion::mouseDoubleClick(const MouseEvent& e)
{
	if (status == Default)
	{
		handleStartEditing(e.getPosition());
	}
}

bool Polytempo_GraphicsEditableRegion::TryGetExistingAnnotation(float x, float y)
{
	if(Polytempo_GraphicsAnnotationManager::getInstance()->getAnchorFlag())
	{
		Polytempo_GraphicsAnnotation* nearestAnnotation = nullptr;
		float nearestAnnotationDistance = FLT_MAX;
		
		// find nearest annotation in anchor mode
		for(int i = 0; i < annotations.size(); i++)
		{
			float dist = annotations[i]->referencePoint.getDistanceFrom(Point<float>(x, y));
			if(dist < nearestAnnotationDistance)
			{
				nearestAnnotation = annotations[i];
				nearestAnnotationDistance = dist;
			}
		}

		if(nearestAnnotationDistance < 15.0f)
		{
			if(Polytempo_GraphicsAnnotationManager::getInstance()->removeAnnotation(nearestAnnotation->id, &temporaryAnnotation))
				return true;

			return false;
		}

		return false;
	}

	return false;
}

void Polytempo_GraphicsEditableRegion::handleStartEditing(Point<int> mousePosition)
{
	float x = float(mousePosition.getX());
	float y = float(mousePosition.getY());
	screenToImage.transformPoint<float>(x, y); 
	

	if (!TryGetExistingAnnotation(x, y))
	{
		temporaryAnnotation.clear();
		temporaryAnnotation.id = Uuid();
		temporaryAnnotation.imageId = currentImageId;
		temporaryAnnotation.referencePoint = Point<float>(x, y);
		temporaryAnnotation.color = colorSelector->getCurrentColour();
		temporaryAnnotation.fontSize = STANDARD_FONT_SIZE;
	}
	
	if (!temporaryAnnotation.freeHandPath.isEmpty())
		temporaryAnnotation.freeHandPath.startNewSubPath(Point<float>(x, y));

	status = FreehandEditing;
	setMouseCursor(MouseCursor::CrosshairCursor);

	buttonsAboveReferencePoint = (mousePosition.getY() > getHeight() - 65);

	buttonOk->setCentrePosition(mousePosition.translated(15, buttonsAboveReferencePoint ? -50 : 50));
	buttonOk->setVisible(true);
	buttonCancel->setCentrePosition(mousePosition.translated(45, buttonsAboveReferencePoint ? -50 : 50));
	buttonCancel->setVisible(true);
	buttonColor->setCentrePosition(mousePosition.translated(0, buttonsAboveReferencePoint ? -80 : 80));
	buttonColor->setVisible(true);
	buttonTextSize->setCentrePosition(mousePosition.translated(30, buttonsAboveReferencePoint ? -80 : 80));
	buttonTextSize->setVisible(true);
	buttonSettings->setCentrePosition(mousePosition.translated(60, buttonsAboveReferencePoint ? -80 : 80));
	buttonSettings->setVisible(true);

	colorSelector->setTopLeftPosition(mousePosition.getX() > getBounds().getWidth() / 2 ? 0 : getBounds().getWidth() - colorSelector->getWidth(), getBounds().getHeight() - colorSelector->getHeight());
	
	grabKeyboardFocus();

	repaintRequired = true;
}

void Polytempo_GraphicsEditableRegion::handleEndEditAccept()
{
	if (!temporaryAnnotation.freeHandPath.isEmpty() || !temporaryAnnotation.text.isEmpty())
		annotations.add(new Polytempo_GraphicsAnnotation(temporaryAnnotation));

	Polytempo_GraphicsAnnotationManager::getInstance()->addAnnotation(temporaryAnnotation);

	handleEndEdit();
}

void Polytempo_GraphicsEditableRegion::handleEndEditCancel()
{
	temporaryAnnotation.clear();
	handleEndEdit();
}

void Polytempo_GraphicsEditableRegion::handleEndEdit()
{
	status = Default;
	setMouseCursor(MouseCursor::NormalCursor);
	buttonOk->setVisible(false);
	buttonCancel->setVisible(false);
	buttonColor->setVisible(false);
	buttonTextSize->setVisible(false);
	buttonSettings->setVisible(false);
	colorSelector->setVisible(false);
	repaintRequired = true;
}

void Polytempo_GraphicsEditableRegion::timerCallback()
{
	if(repaintRequired)
	{
		repaintRequired = false;
		stopTimer();
		repaint();
		startTimer(MIN_INTERVAL_BETWEEN_REPAINTS_MS);
	}
}

void Polytempo_GraphicsEditableRegion::AddFontSizeToMenu(PopupMenu* m, int fontSize) const
{
	m->addItem(fontSize, std::to_string(fontSize), true, temporaryAnnotation.fontSize == fontSize);
}

PopupMenu Polytempo_GraphicsEditableRegion::getTextSizePopupMenu() const
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

void Polytempo_GraphicsEditableRegion::buttonClicked(Button* source)
{
	if(source == buttonOk)
	{
		handleEndEditAccept();
	}
	else if(source == buttonCancel)
	{
		handleEndEditCancel();
	}
	else if(source == buttonColor)
	{
		colorSelector->setVisible(!colorSelector->isVisible());
	}
	else if(source == buttonTextSize)
	{
		getTextSizePopupMenu().showMenuAsync(PopupMenu::Options().withTargetComponent(buttonTextSize), new FontSizeCallback(this));
	}
	else if(source == buttonSettings)
	{
		Polytempo_GraphicsAnnotationManager::getInstance()->showSettingsDialog();
	}
}

void Polytempo_GraphicsEditableRegion::changeListenerCallback(ChangeBroadcaster* source)
{
	if(source == colorSelector && status != Default)
	{
		temporaryAnnotation.color = colorSelector->getCurrentColour();
		repaintRequired = true;
	}
	else if(source == Polytempo_GraphicsAnnotationManager::getInstance())
	{
		Polytempo_GraphicsAnnotationManager::getInstance()->getAnnotationsForImage(currentImageId, &annotations);
		repaintRequired = true;
	}
}

bool Polytempo_GraphicsEditableRegion::keyPressed(const KeyPress& key, Component* /*originatingComponent*/)
{
	if(status != Default && key.isValid())
	{
		juce_wchar character = key.getTextCharacter();
		String str;
		str += character;
		if (!str.isEmpty())
			temporaryAnnotation.text.append(str, 1);
		repaintRequired = true;

		return true;
	}

	return false;
}

void Polytempo_GraphicsEditableRegion::setTemporaryFontSize(int fontSize)
{
	temporaryAnnotation.fontSize = fontSize;
	repaintRequired = true;
}
