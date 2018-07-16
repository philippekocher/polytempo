/*
  ==============================================================================

    Polytempo_GraphicsEditableRegion.cpp
    Created: 27 Jul 2017 8:40:18am
    Author:  chris

  ==============================================================================
*/

#include "JuceHeader.h"
#include "Polytempo_GraphicsEditableRegion.h"
#include "Polytempo_GraphicsAnnotationManager.h"

#include <string>

//==============================================================================
Polytempo_GraphicsEditableRegion::Polytempo_GraphicsEditableRegion()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
	status = Default;
	allowAnnotations = false;

	palette = new Polytempo_GraphicsPalette(this);

	addKeyListener(this);

	Polytempo_GraphicsAnnotationManager::getInstance()->addChangeListener(this);
	startTimer(TIMER_ID_REPAINT, MIN_INTERVAL_BETWEEN_REPAINTS_MS);
}

Polytempo_GraphicsEditableRegion::~Polytempo_GraphicsEditableRegion()
{
	Polytempo_GraphicsAnnotationManager::getInstance()->removeChangeListener(this);
	Polytempo_GraphicsAnnotationManager::getInstance()->resetAnnotationPending(this);
	stopTimer(TIMER_ID_REPAINT);
	stopTimer(TIMER_ID_AUTO_ACCEPT);
}


void Polytempo_GraphicsEditableRegion::prepareAnnotationLayer()
{
	annotationImage = new Image(Image::ARGB, int(imageWidth), int(imageHeight), true);

	Graphics g(*annotationImage);
	bool anchorFlag = Polytempo_GraphicsAnnotationManager::getInstance()->getAnchorFlag();
	for (Polytempo_GraphicsAnnotation* annotation : annotations)
	{
		if (currentImageRectangle.contains(annotation->referencePoint))
			paintAnnotation(g, annotation, anchorFlag, Colours::blue);
	}
}

void Polytempo_GraphicsEditableRegion::paintAnnotation(Graphics& g, const Polytempo_GraphicsAnnotation* annotation, bool anchorFlag, Colour anchorColor)
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

	if(anchorFlag)
	{		
        g.setColour(anchorColor);
		g.fillRoundedRectangle(annotation->referencePoint.getX() - 8.0f, annotation->referencePoint.getY() - 8.0f, 16.0f, 16.0f, 4.0f);
	}
}

void Polytempo_GraphicsEditableRegion::paint (Graphics& g)
{
	bool doRepaint = repaintRequired.compareAndSetBool(false, true);
	bool doFullRepaint = fullRepaintRequired.compareAndSetBool(false, true);
	if (doRepaint || doFullRepaint)
	{

		if (doFullRepaint)
		{
			prepareAnnotationLayer();
			paintContent(g);

			if (annotationImage != nullptr)
			{
				g.drawImage(*annotationImage,
					targetArea.getX(), targetArea.getY(), targetArea.getWidth(), targetArea.getHeight(),
					(int)0, (int)0, (int)annotationImage->getWidth(), (int)annotationImage->getHeight());
			}
		}
	}

	if (status == FreehandEditing)
	{
		g.addTransform(imageToScreen);

		paintAnnotation(g, &temporaryAnnotation, true, Colours::red);
	}
}

void Polytempo_GraphicsEditableRegion::resized()
{
	resizeContent();

	if (currentImageRectangle.isEmpty())
		allowAnnotations = false;
	else
	{
		screenToImage = AffineTransform::translation(-float(getX()), -float(getY()));
		screenToImage = screenToImage.scale(currentImageRectangle.getWidth() / float(targetArea.getWidth()), currentImageRectangle.getHeight() / float(targetArea.getHeight()));
		screenToImage = screenToImage.translated(currentImageRectangle.getX(), currentImageRectangle.getY() - targetArea.getY() / float(targetArea.getHeight()) * currentImageRectangle.getHeight());

		imageToScreen = screenToImage.inverted();
		allowAnnotations = true;
	}

	fullRepaintRequired.set(true);
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

	if(status == FreehandEditing)
	{
		palette->show(true);
		startTimer(TIMER_ID_AUTO_ACCEPT, AUTO_ACCEPT_INTERVAL_MS);
	}
}

void Polytempo_GraphicsEditableRegion::mouseDown(const MouseEvent& e)
{
	if (status == Default)
	{
		handleStartEditing(e.getPosition());
	}
	if(status == FreehandEditing)
	{
		float x = float(e.getPosition().getX());
		float y = float(e.getPosition().getY());
		screenToImage.transformPoint(x, y);

		temporaryAnnotation.freeHandPath.startNewSubPath(Point<float>(x, y));

		stopTimer(TIMER_ID_AUTO_ACCEPT);
	}
}

void Polytempo_GraphicsEditableRegion::handleFreeHandPainting(const Point<int>& mousePosition)
{
	if (temporaryAnnotation.freeHandPath.isEmpty() || lastPathPoint.getDistanceFrom(mousePosition) > 1)
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
		repaintRequired.set(true);
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
	if (!allowAnnotations || !Polytempo_GraphicsAnnotationManager::getInstance()->trySetAnnotationPending(this))
		return;

	float x = float(mousePosition.getX());
	float y = float(mousePosition.getY());
	screenToImage.transformPoint<float>(x, y); 
	

	if (!TryGetExistingAnnotation(x, y))
	{
		temporaryAnnotation.clear();
		temporaryAnnotation.id = Uuid();
		temporaryAnnotation.imageId = currentImageId;
		temporaryAnnotation.referencePoint = Point<float>(x, y);
		temporaryAnnotation.color = palette->getCurrentColour();
		temporaryAnnotation.fontSize = STANDARD_FONT_SIZE;
	}
	
	if (!temporaryAnnotation.freeHandPath.isEmpty())
		temporaryAnnotation.freeHandPath.startNewSubPath(Point<float>(x, y));

	status = FreehandEditing;
	setMouseCursor(MouseCursor::CrosshairCursor);

	grabKeyboardFocus();

	repaintRequired.set(true);
}

void Polytempo_GraphicsEditableRegion::handleEndEditAccept()
{
	if (temporaryAnnotation.freeHandPath.isEmpty() && temporaryAnnotation.text.isEmpty())
		return; // noting to add

	Polytempo_GraphicsAnnotationManager::getInstance()->addAnnotation(temporaryAnnotation);
	handleEndEdit();
}

void Polytempo_GraphicsEditableRegion::handleEndEditCancel()
{
	temporaryAnnotation.clear();
	handleEndEdit();
}

int Polytempo_GraphicsEditableRegion::getTemporaryFontSize() const
{
	return temporaryAnnotation.fontSize;
}

void Polytempo_GraphicsEditableRegion::setTemporaryColor(Colour colour)
{
	if (status != Default)
	{
		temporaryAnnotation.color = colour;
		fullRepaintRequired.set(true);
	}
}

void Polytempo_GraphicsEditableRegion::stopAutoAccept()
{
	stopTimer(TIMER_ID_AUTO_ACCEPT);
}

void Polytempo_GraphicsEditableRegion::hitBtnColor() const
{
	palette->hitBtnColor();
}

void Polytempo_GraphicsEditableRegion::hitBtnTextSize() const
{
	palette->hitBtnTextSize();
}

void Polytempo_GraphicsEditableRegion::handleEndEdit()
{
	status = Default;
	setMouseCursor(MouseCursor::NormalCursor);
	palette->show(false);
	
	Polytempo_GraphicsAnnotationManager::getInstance()->resetAnnotationPending(this);
	fullRepaintRequired.set(true);
}

void Polytempo_GraphicsEditableRegion::changeListenerCallback(ChangeBroadcaster* source)
{
	if(source == Polytempo_GraphicsAnnotationManager::getInstance())
	{
		Polytempo_GraphicsAnnotationManager::getInstance()->getAnnotationsForImage(currentImageId, &annotations);
		fullRepaintRequired.set(true);
	}
}

bool Polytempo_GraphicsEditableRegion::keyPressed(const KeyPress& key, Component* /*originatingComponent*/)
{
	if(status != Default && key.isValid())
	{
		if (key == KeyPress::escapeKey)
			handleEndEditCancel();
		else if (key == KeyPress::returnKey)
			handleEndEditAccept();
		else if(key == KeyPress::backspaceKey)
		{
			if (temporaryAnnotation.text.length() > 0)
				temporaryAnnotation.text = temporaryAnnotation.text.dropLastCharacters(1);
			startTimer(TIMER_ID_AUTO_ACCEPT, AUTO_ACCEPT_INTERVAL_MS);
			fullRepaintRequired.set(true);
		}
		else
		{
			juce_wchar character = key.getTextCharacter();
			String str;
			str += character;
			if (!str.isEmpty())
				temporaryAnnotation.text.append(str, 1);
			startTimer(TIMER_ID_AUTO_ACCEPT, AUTO_ACCEPT_INTERVAL_MS);
			repaintRequired.set(true);
		}
		return true;
	}

	return false;
}

void Polytempo_GraphicsEditableRegion::setTemporaryFontSize(float fontSize)
{
	temporaryAnnotation.fontSize = fontSize;
	fullRepaintRequired.set(true);
}

void Polytempo_GraphicsEditableRegion::timerCallback(int timerID)
{
	switch (timerID)
	{
	case TIMER_ID_REPAINT:
		stopTimer(timerID);
		repaint();
		startTimer(timerID, MIN_INTERVAL_BETWEEN_REPAINTS_MS);
		break;
	case TIMER_ID_AUTO_ACCEPT:
		stopTimer(timerID);
		if (status == FreehandEditing)
			handleEndEditAccept();
		break;
	default:
		stopTimer(timerID);
	}
}
