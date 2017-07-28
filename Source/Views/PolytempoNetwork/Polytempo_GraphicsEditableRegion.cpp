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
	Colour overColour = Colours::purple;
	Colour downColour = Colours::green;
	float overOpacity = 0.2;
	float downOpacity = 0.5;
	
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
	buttonOk->setImages(false, false, false, imageOk, 1.0f, Colours::transparentWhite, imageOk, overOpacity, overColour, imageOk, downOpacity, downColour);
	buttonOk->setBounds(0, 0, width, height);
	buttonOk->addListener(this);
	addChildComponent(buttonOk);
	
	buttonCancel = new ImageButton("Cancel");
	buttonCancel->setImages(false, false, false, imageCancel, 1.0f, Colours::transparentWhite, imageCancel, overOpacity, overColour, imageCancel, downOpacity, downColour);
	buttonCancel->setBounds(0, 0, 30, 30);
	buttonCancel->addListener(this);
	addChildComponent(buttonCancel);

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
		g.drawArrow(Line<int>(referencePoint.translated(0, buttonsAboveReferencePoint ? -50 : 50), referencePoint).toFloat(), 4, 8, 8);

	g.setColour(Colours::red);
	g.strokePath(temporaryFreeHandPath, type);

	g.setColour(Colours::blue);
	g.strokePath(freeHandPath, type);
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
		temporaryFreeHandPath.startNewSubPath(e.getPosition().toFloat());
	}
}

void Polytempo_GraphicsEditableRegion::handleFreeHandPainting(const Point<int>& mousePosition)
{
	if (temporaryFreeHandPath.isEmpty() || lastPathPoint.getDistanceFrom(mousePosition) > 5)
	{
		if (temporaryFreeHandPath.isEmpty())
			temporaryFreeHandPath.addLineSegment(Line<float>(mousePosition.toFloat(), mousePosition.toFloat()), FREE_HAND_LINE_THICKNESS);
		else
			temporaryFreeHandPath.lineTo(mousePosition.toFloat());

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
	temporaryFreeHandPath.clear();
	status = FreehandEditing;
	setMouseCursor(MouseCursor::CrosshairCursor);

	buttonsAboveReferencePoint = (referencePoint.getY() > getHeight() - 65);

	buttonOk->setCentrePosition(referencePoint.translated(15, buttonsAboveReferencePoint ? -50 : 50));
	buttonOk->setVisible(true);
	buttonCancel->setCentrePosition(referencePoint.translated(45, buttonsAboveReferencePoint ? -50 : 50));
	buttonCancel->setVisible(true);
	repaintRequired = true;
}

void Polytempo_GraphicsEditableRegion::handleEndEditAccept()
{
	if(!temporaryFreeHandPath.isEmpty())
		freeHandPath.addPath(temporaryFreeHandPath);
	handleEndEdit();
}

void Polytempo_GraphicsEditableRegion::handleEndEditCancel()
{
	temporaryFreeHandPath.clear();
	handleEndEdit();
}

void Polytempo_GraphicsEditableRegion::handleEndEdit()
{
	status = Default;
	setMouseCursor(MouseCursor::NormalCursor);
	buttonOk->setVisible(false);
	buttonCancel->setVisible(false);
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
