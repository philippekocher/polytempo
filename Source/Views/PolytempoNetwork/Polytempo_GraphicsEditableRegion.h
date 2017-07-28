/*
  ==============================================================================

    Polytempo_GraphicsEditableRegion.h
    Created: 27 Jul 2017 8:40:18am
    Author:  chris

  ==============================================================================
*/

#pragma once

#include "../../../POLYTEMPO NETWORK/JuceLibraryCode/JuceHeader.h"
#include "Polytempo_GraphicsAnnotation.h"

#define MIN_MOUSE_DOWN_TIME_MS 500
#define MIN_INTERVAL_BETWEEN_REPAINTS_MS 100
#define FREE_HAND_LINE_THICKNESS 2

//==============================================================================
/*
*/
class Polytempo_GraphicsEditableRegion    : public Component, Timer, ButtonListener, ChangeListener
{
public:
    Polytempo_GraphicsEditableRegion();
    ~Polytempo_GraphicsEditableRegion();

	virtual void paintContent(Graphics& g) = 0;
	
    void paint (Graphics&) override;
	
	void setImage(Image *img, var rect, String imageId);
	void mouseUp(const MouseEvent& e) override;
	void mouseDown(const MouseEvent& e) override;
	void mouseDrag(const MouseEvent& e) override;
	void mouseDoubleClick(const MouseEvent& e) override;
	void mouseEnter(const MouseEvent& e) override;
	void timerCallback() override;
	void buttonClicked(Button* source) override;
	void changeListenerCallback(ChangeBroadcaster* source) override;

private:
	virtual void setViewImage(Image *img, var) = 0;
	void handleStartEditing(Point<int> mousePosition);
	void handleEndEditAccept();
	void handleEndEditCancel();
	void handleEndEdit();
	void handleFreeHandPainting(const Point<int>& mousePosition);

	enum Status { Default, FreehandEditing } status;
	Array<Polytempo_GraphicsAnnotation> annotations;
	Polytempo_GraphicsAnnotation temporaryAnnotation;
	Point<int> referencePoint;
	Point<int> lastPathPoint;
	bool repaintRequired;
	
	ScopedPointer<ImageButton> buttonOk;
	ScopedPointer<ImageButton> buttonCancel;
	bool buttonsAboveReferencePoint;

	Rectangle<float> currentViewRectangle;
	String currentImageId;

	ScopedPointer<ColourSelector> colorSelector;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_GraphicsEditableRegion)
	
};
