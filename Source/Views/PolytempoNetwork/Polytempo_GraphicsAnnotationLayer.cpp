/*
  ==============================================================================

    Polytempo_GraphicsAnnotationLayer.cpp
    Created: 22 Jul 2018 5:42:33am
    Author:  chris

  ==============================================================================
*/

#include "JuceHeader.h"
#include "Polytempo_GraphicsAnnotationLayer.h"
#include "Polytempo_GraphicsAnnotationManager.h"

//==============================================================================
Polytempo_GraphicsAnnotationLayer::Polytempo_GraphicsAnnotationLayer(HashMap < String, Polytempo_GraphicsViewRegion* >* pRegionMap)
{
	this->pRegionMap = pRegionMap;
	status = Default;
	setAlwaysOnTop(true);
	addKeyListener(this);
	startTimer(TIMER_ID_REPAINT, MIN_INTERVAL_BETWEEN_REPAINTS_MS);
	palette = new Polytempo_GraphicsPalette(this);

	Polytempo_GraphicsAnnotationManager::getInstance()->addChangeListener(this);
}

Polytempo_GraphicsAnnotationLayer::~Polytempo_GraphicsAnnotationLayer()
{
	stopTimer(TIMER_ID_REPAINT);
	stopTimer(TIMER_ID_AUTO_ACCEPT);
	Polytempo_GraphicsAnnotationManager::getInstance()->removeChangeListener(this);
	Polytempo_GraphicsAnnotationManager::getInstance()->resetAnnotationPending(this);
}

void Polytempo_GraphicsAnnotationLayer::paint (Graphics& g)
{
	bool doFullRepaint = fullUpdateRequired.compareAndSetBool(false, true);
	if (doFullRepaint)
	{
		prepareAnnotationLayer();
	}
	if (annotationImage != nullptr)
	{
		g.drawImage(*annotationImage,
			0, 0, getWidth(), getHeight(),
			int(0), int(0), int(annotationImage->getWidth()), int(annotationImage->getHeight()));
	}
	
	if(status == FreehandEditing && temporaryAnnotation.pRegion != nullptr)
	{
		g.addTransform(temporaryAnnotation.pRegion->getImageToScreenTransform());
		paintAnnotation(g, &temporaryAnnotation, true, Colours::red);
	}
}

void Polytempo_GraphicsAnnotationLayer::resized()
{
	fullUpdateRequired.set(true);
	repaint();
}

void Polytempo_GraphicsAnnotationLayer::prepareAnnotationLayer()
{
	annotationImage = new Image(Image::ARGB, getWidth(), getHeight(), true);

	bool anchorFlag = Polytempo_GraphicsAnnotationManager::getInstance()->getAnchorFlag();
	
	HashMap < String, Polytempo_GraphicsViewRegion* >::Iterator it1(*pRegionMap);
	while (it1.next())
	{
		OwnedArray<Polytempo_GraphicsAnnotation> annotations;
		Polytempo_GraphicsAnnotationManager::getInstance()->getAnnotationsForImage(it1.getValue()->getImageID(), &annotations);
		
		Graphics g(*annotationImage);
		g.addTransform(it1.getValue()->getImageToScreenTransform());
		
		for (Polytempo_GraphicsAnnotation* annotation : annotations)
		{
			if(it1.getValue()->imageRectangleContains(annotation->referencePoint) && (status != FreehandEditing || temporaryAnnotation.id != annotation->id))
				paintAnnotation(g, annotation, anchorFlag, Colours::blue);
		}
	}
}

void Polytempo_GraphicsAnnotationLayer::paintAnnotation(Graphics& g, const Polytempo_GraphicsAnnotation* annotation, bool anchorFlag, Colour anchorColor)
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

	if (anchorFlag)
	{
		g.setColour(anchorColor);
		g.fillRoundedRectangle(annotation->referencePoint.getX() - 8.0f, annotation->referencePoint.getY() - 8.0f, 16.0f, 16.0f, 4.0f);
	}
}

void Polytempo_GraphicsAnnotationLayer::requireUpdate()
{
	fullUpdateRequired.set(true);
}

Polytempo_GraphicsViewRegion* Polytempo_GraphicsAnnotationLayer::getRegionAt(Point<int> point) const
{
	HashMap < String, Polytempo_GraphicsViewRegion* >::Iterator it1(*pRegionMap);
	while (it1.next())
	{
		Polytempo_GraphicsViewRegion* pRegion = it1.getValue();
		if (pRegion->getBounds().contains(point.getX(), point.getY()))
		{
			return pRegion;
		}
	}

	return nullptr;
}

void Polytempo_GraphicsAnnotationLayer::handleStartEditing(Point<int> mousePosition)
{
	Polytempo_GraphicsViewRegion * pRegion = getRegionAt(mousePosition);
	
	if (pRegion == nullptr || !pRegion->annotationsAllowed())
		return;

	if (!Polytempo_GraphicsAnnotationManager::getInstance()->trySetAnnotationPending(this))
		return;

	Point<float> imageCoordiantes = pRegion->getImageCoordinatesAt(mousePosition);	
	
	if (!tryGetExistingAnnotation(imageCoordiantes, pRegion))
	{
		temporaryAnnotation.clear();
		temporaryAnnotation.id = Uuid();
		temporaryAnnotation.imageId = pRegion->getImageID();
		temporaryAnnotation.referencePoint = pRegion->getImageCoordinatesAt(mousePosition);
		temporaryAnnotation.color = palette->getCurrentColour();
		temporaryAnnotation.fontSize = STANDARD_FONT_SIZE;
		temporaryAnnotation.pRegion = pRegion;
	}

	if (!temporaryAnnotation.freeHandPath.isEmpty())
		temporaryAnnotation.freeHandPath.startNewSubPath(mousePosition.toFloat());

	status = FreehandEditing;
	setMouseCursor(MouseCursor::CrosshairCursor);

	grabKeyboardFocus();
	repaint();
}

void Polytempo_GraphicsAnnotationLayer::handleFreeHandPainting(const Point<int>& mousePosition)
{
	if (temporaryAnnotation.freeHandPath.isEmpty() || lastPathPoint.getDistanceFrom(mousePosition) > 1)
	{
		Point<float> imagePt = temporaryAnnotation.pRegion->getImageCoordinatesAt(mousePosition);
		float x = imagePt.getX();
		float y = imagePt.getY();

		if (temporaryAnnotation.freeHandPath.isEmpty())
		{
			temporaryAnnotation.freeHandPath.addLineSegment(Line<float>(x, y, x, y), FREE_HAND_LINE_THICKNESS);
		}
		else
		{
			temporaryAnnotation.freeHandPath.lineTo(x, y);
		}

		lastPathPoint = mousePosition;
		repaint();
	}
}

void Polytempo_GraphicsAnnotationLayer::handleEndEditAccept()
{
	if (temporaryAnnotation.freeHandPath.isEmpty() && temporaryAnnotation.text.isEmpty())
		return; // noting to add

	Polytempo_GraphicsAnnotationManager::getInstance()->addAnnotation(temporaryAnnotation);
	handleEndEdit();
}

void Polytempo_GraphicsAnnotationLayer::handleEndEditCancel()
{
	temporaryAnnotation.clear();
	handleEndEdit();
}

void Polytempo_GraphicsAnnotationLayer::handleEndEdit()
{
	status = Default;
	setMouseCursor(MouseCursor::NormalCursor);
	palette->show(false);

	Polytempo_GraphicsAnnotationManager::getInstance()->resetAnnotationPending(this);
	fullUpdateRequired.set(true);
}

void Polytempo_GraphicsAnnotationLayer::handleDeleteSelected()
{
	Polytempo_GraphicsAnnotationManager::getInstance()->removeAnnotation(temporaryAnnotation.id);
	handleEndEditCancel();
}

void Polytempo_GraphicsAnnotationLayer::mouseDown(const MouseEvent& event)
{
	if (status == Default)
	{
		handleStartEditing(event.getPosition());
	}
	else if (status == FreehandEditing)
	{
		Polytempo_GraphicsViewRegion* pRegion = getRegionAt(event.getPosition());
		if (pRegion == nullptr || pRegion->getImageID() != temporaryAnnotation.imageId || pRegion != temporaryAnnotation.pRegion)
		{
			handleEndEditAccept();
			handleStartEditing(event.getPosition());
		}
		else
		{
			temporaryAnnotation.freeHandPath.startNewSubPath(pRegion->getImageCoordinatesAt(event.getPosition()));
		}
		stopTimer(TIMER_ID_AUTO_ACCEPT);
	}
}

void Polytempo_GraphicsAnnotationLayer::mouseUp(const MouseEvent& event)
{
	if (status == Default)
	{
		if (event.getLengthOfMousePress() > MIN_MOUSE_DOWN_TIME_MS)
		{
			handleStartEditing(event.getPosition());
		}
	}

	if (status == FreehandEditing)
	{
		palette->show(true);
		startTimer(TIMER_ID_AUTO_ACCEPT, AUTO_ACCEPT_INTERVAL_MS);
	}
}

void Polytempo_GraphicsAnnotationLayer::mouseDrag(const MouseEvent& event)
{
	if (status == FreehandEditing)
	{
		handleFreeHandPainting(event.getPosition());
	}
}

void Polytempo_GraphicsAnnotationLayer::mouseDoubleClick(const MouseEvent& event)
{
	if (status == Default)
	{
		handleStartEditing(event.getPosition());
	}
}

void Polytempo_GraphicsAnnotationLayer::timerCallback(int timerID)
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

bool Polytempo_GraphicsAnnotationLayer::keyPressed(const KeyPress& key, Component* /*originatingComponent*/)
{
	if (status != Default && key.isValid())
	{
		if (key == KeyPress::escapeKey)
			handleEndEditCancel();
		else if (key == KeyPress::returnKey)
			handleEndEditAccept();
		else if (key == KeyPress::backspaceKey)
		{
			if (temporaryAnnotation.text.length() > 0)
				temporaryAnnotation.text = temporaryAnnotation.text.dropLastCharacters(1);
			startTimer(TIMER_ID_AUTO_ACCEPT, AUTO_ACCEPT_INTERVAL_MS);
			fullUpdateRequired.set(true);
		}
		else if (key == KeyPress::deleteKey)
		{
			return false;
		}
		else
		{
			juce_wchar character = key.getTextCharacter();
			String str;
			str += character;
			if (!str.isEmpty())
				temporaryAnnotation.text.append(str, 1);
			startTimer(TIMER_ID_AUTO_ACCEPT, AUTO_ACCEPT_INTERVAL_MS);
			repaint();
		}
		return true;
	}

	return false;
}

int Polytempo_GraphicsAnnotationLayer::getTemporaryFontSize() const
{
	return int(temporaryAnnotation.fontSize);
}

void Polytempo_GraphicsAnnotationLayer::setTemporaryColor(Colour colour)
{
	if (status != Default)
	{
		temporaryAnnotation.color = colour;
		fullUpdateRequired.set(true);
	}
}

void Polytempo_GraphicsAnnotationLayer::stopAutoAccept()
{
	stopTimer(TIMER_ID_AUTO_ACCEPT);
}

void Polytempo_GraphicsAnnotationLayer::hitBtnColor() const
{
	palette->hitBtnColor();
}

void Polytempo_GraphicsAnnotationLayer::hitBtnTextSize() const
{
	palette->hitBtnTextSize();
}

void Polytempo_GraphicsAnnotationLayer::setTemporaryFontSize(float fontSize)
{
	temporaryAnnotation.fontSize = fontSize;
	fullUpdateRequired.set(true);
}

void Polytempo_GraphicsAnnotationLayer::changeListenerCallback(ChangeBroadcaster* source)
{
	if (source == Polytempo_GraphicsAnnotationManager::getInstance())
	{
		fullUpdateRequired.set(true);
	}
}


bool Polytempo_GraphicsAnnotationLayer::tryGetExistingAnnotation(Point<float> point, Polytempo_GraphicsViewRegion* pRegion)
{
	if (Polytempo_GraphicsAnnotationManager::getInstance()->getAnchorFlag())
	{
		Polytempo_GraphicsAnnotation* nearestAnnotation = nullptr;
		float nearestAnnotationDistance = FLT_MAX;

		OwnedArray<Polytempo_GraphicsAnnotation> annotations;
		Polytempo_GraphicsAnnotationManager::getInstance()->getAnnotationsForImage(pRegion->getImageID(), &annotations);

		// find nearest annotation in anchor mode
		for (int i = 0; i < annotations.size(); i++)
		{
			float dist = annotations[i]->referencePoint.getDistanceFrom(Point<float>(point.getX(), point.getY()));
			if (dist < nearestAnnotationDistance)
			{
				nearestAnnotation = annotations[i];
				nearestAnnotationDistance = dist;
			}
		}

		if (nearestAnnotationDistance < 15.0f)
		{
			if (Polytempo_GraphicsAnnotationManager::getInstance()->getAnnotation(nearestAnnotation->id, &temporaryAnnotation))
			{
				temporaryAnnotation.pRegion = pRegion;
				return true;
			}

			return false;
		}

		return false;
	}

	return false;
}
