/*
  ==============================================================================

    Polytempo_GraphicsEditableRegion.cpp
    Created: 27 Jul 2017 8:40:18am
    Author:  chris

  ==============================================================================
*/

#include "../../../POLYTEMPO NETWORK/JuceLibraryCode/JuceHeader.h"
#include "Polytempo_GraphicsEditableRegion.h"

//==============================================================================
Polytempo_GraphicsEditableRegion::Polytempo_GraphicsEditableRegion()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
	status = Default;
	
	Colour background = Colours::lightgrey;
	Colour border = Colours::black;
	int width = 30;
	int height = 30;
	float borderThickness = 2.0f;
	Colour overColour = Colour(Colours::purple.getRed(), Colours::purple.getGreen(), Colours::purple.getBlue(), uint8(80));
	float overOpacity = 1.0;
	float downOpacity = 1.0;
	
	Image imageBase(Image::RGB, width, height, true);
	Graphics g(imageBase);
	g.fillAll(background);
	g.setColour(border);
	g.drawRect(0.0f, 0.0f, float(width), float(height), borderThickness);

	Image imageOk(imageBase);
	imageOk.duplicateIfShared();
	Graphics gOk(imageOk);
	gOk.setColour(Colours::green); 
	gOk.drawLine(25, 5, 15, 25, 4);
	gOk.drawLine(15, 25, 10, 15, 4);
	
	Image imageCancel(imageBase);
	imageCancel.duplicateIfShared();
	Graphics gCancel(imageCancel);
	gCancel.setColour(Colours::red);
	gCancel.drawLine(5, 5, 25, 25, 4);
	gCancel.drawLine(25, 5, 5, 25, 4);

	buttonOk = new ImageButton("OK");
	buttonOk->setImages(false, false, false, imageOk, 1.0f, Colours::transparentWhite, Image::null, overOpacity, overColour, Image::null, downOpacity, Colours::green);
	buttonOk->setBounds(0, 0, width, height);
	buttonOk->addListener(this);
	addChildComponent(buttonOk);
	
	buttonCancel = new ImageButton("Cancel");
	buttonCancel->setImages(false, false, false, imageCancel, 1.0f, Colours::transparentWhite, Image::null, overOpacity, overColour, Image::null, downOpacity, Colours::red);
	buttonCancel->setBounds(0, 0, 30, 30);
	buttonCancel->addListener(this);
	addChildComponent(buttonCancel);

	colorSelector = new ColourSelector(ColourSelector::ColourSelectorOptions::showAlphaChannel | ColourSelector::ColourSelectorOptions::showColourspace);
	colorSelector->setBounds(0, 0, 200, 200);
	colorSelector->setCurrentColour(Colours::red);
	colorSelector->addChangeListener(this);
	addChildComponent(colorSelector);

	startTimer(MIN_INTERVAL_BETWEEN_REPAINTS_MS);
}

Polytempo_GraphicsEditableRegion::~Polytempo_GraphicsEditableRegion()
{
}

void Polytempo_GraphicsEditableRegion::paint (Graphics& g)
{
	paintContent(g);

	PathStrokeType type(PathStrokeType::rounded);
	
	if (status == FreehandEditing)
	{
		g.drawArrow(Line<int>(referencePoint.translated(0, buttonsAboveReferencePoint ? -50 : 50), referencePoint).toFloat(), 4, 8, 8);

		g.setColour(temporaryAnnotation.color);

		if(!temporaryAnnotation.freeHandPath.isEmpty())
			g.strokePath(temporaryAnnotation.freeHandPath, type);
	}

	for (Polytempo_GraphicsAnnotation annotation : annotations)
	{
		g.setColour(annotation.color);
		g.strokePath(annotation.freeHandPath, type);
	}
}

void Polytempo_GraphicsEditableRegion::setImage(Image* img, var rect, String imageId)
{
	Array < var > r = *rect.getArray();
	currentViewRectangle = Rectangle<float>(Point<float>(r[0], r[1]), Point<float>(r[2], r[3]));
	currentImageId = imageId;

	setViewImage(img, rect);
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
		temporaryAnnotation.freeHandPath.startNewSubPath(e.getPosition().toFloat());
	}
}

void Polytempo_GraphicsEditableRegion::handleFreeHandPainting(const Point<int>& mousePosition)
{
	if (temporaryAnnotation.freeHandPath.isEmpty() || lastPathPoint.getDistanceFrom(mousePosition) > 5)
	{
		if (temporaryAnnotation.freeHandPath.isEmpty())
			temporaryAnnotation.freeHandPath.addLineSegment(Line<float>(mousePosition.toFloat(), mousePosition.toFloat()), FREE_HAND_LINE_THICKNESS);
		else
			temporaryAnnotation.freeHandPath.lineTo(mousePosition.toFloat());

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

void Polytempo_GraphicsEditableRegion::mouseEnter(const MouseEvent& e)
{
	
}

void Polytempo_GraphicsEditableRegion::handleStartEditing(Point<int> mousePosition)
{
	referencePoint = mousePosition;
	temporaryAnnotation.referencePoint = mousePosition.toFloat();	// todo transform!
	temporaryAnnotation.clear();
	temporaryAnnotation.color = colorSelector->getCurrentColour();
	status = FreehandEditing;
	setMouseCursor(MouseCursor::CrosshairCursor);

	buttonsAboveReferencePoint = (referencePoint.getY() > getHeight() - 65);

	buttonOk->setCentrePosition(referencePoint.translated(15, buttonsAboveReferencePoint ? -50 : 50));
	buttonOk->setVisible(true);
	buttonCancel->setCentrePosition(referencePoint.translated(45, buttonsAboveReferencePoint ? -50 : 50));
	buttonCancel->setVisible(true);

	colorSelector->setCentrePosition(referencePoint.getX() - 150, referencePoint.getY());
	colorSelector->setVisible(true);
	repaintRequired = true;
}

void Polytempo_GraphicsEditableRegion::handleEndEditAccept()
{
	if (!temporaryAnnotation.freeHandPath.isEmpty())
		annotations.add(temporaryAnnotation);
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
}

void Polytempo_GraphicsEditableRegion::changeListenerCallback(ChangeBroadcaster* source)
{
	if(status != Default)
	{
		temporaryAnnotation.color = colorSelector->getCurrentColour();
	}
}
