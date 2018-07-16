/*
  ==============================================================================

    Polytempo_GraphicsEditableRegion.h
    Created: 27 Jul 2017 8:40:18am
    Author:  chris

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "Polytempo_GraphicsAnnotation.h"
#include "Polytempo_GraphicsPalette.h"

#define MIN_MOUSE_DOWN_TIME_MS 500
#define MIN_INTERVAL_BETWEEN_REPAINTS_MS 20
#define FREE_HAND_LINE_THICKNESS 2
#define DISTANCE_REFERENCEPOINT_TO_BUTTONS 50
#define TIMER_ID_REPAINT 1000
#define TIMER_ID_AUTO_ACCEPT 1001
#define AUTO_ACCEPT_INTERVAL_MS	5000

class FontSizeCallback;

//==============================================================================
/*
*/
class Polytempo_GraphicsEditableRegion : public Component, MultiTimer, KeyListener, ChangeListener
{
public:
	Polytempo_GraphicsEditableRegion();
    ~Polytempo_GraphicsEditableRegion();
	
	virtual void paintContent(Graphics& g) = 0;
	virtual void resizeContent() = 0;

	void paint (Graphics&) override;
	void resized() override;
	void paintAnnotation(Graphics& g, const Polytempo_GraphicsAnnotation* annotation, bool anchorFlag, Colour anchorColor);

	void setImage(Image *img, var rect, String imageId);
	void mouseUp(const MouseEvent& e) override;
	void mouseDown(const MouseEvent& e) override;
	void mouseDrag(const MouseEvent& e) override;
	void mouseDoubleClick(const MouseEvent& e) override;
	bool TryGetExistingAnnotation(float x, float y);
	void changeListenerCallback(ChangeBroadcaster* source) override;
	bool keyPressed(const KeyPress& key, Component* /*originatingComponent*/) override;
	void setTemporaryFontSize(float fontSize);
	void timerCallback(int timerID) override;
	void handleEndEditAccept();
	void handleEndEditCancel();
	int getTemporaryFontSize() const;
	void setTemporaryColor(Colour colour);
	void stopAutoAccept();
	void hitBtnColor() const;
	void hitBtnTextSize() const;

private:
	virtual void setViewImage(Image *img, var) = 0;
	void handleStartEditing(Point<int> mousePosition);
	void handleEndEdit();
	void handleFreeHandPainting(const Point<int>& mousePosition);
	void prepareAnnotationLayer();
    
protected:
	Rectangle<int> targetArea;

	enum Status { Default, FreehandEditing } status;
	OwnedArray<Polytempo_GraphicsAnnotation> annotations;
	Polytempo_GraphicsAnnotation temporaryAnnotation;
	Point<int> lastPathPoint;
	Atomic<bool> repaintRequired;
	Atomic<bool> fullRepaintRequired;
	bool allowAnnotations;
	ScopedPointer<Image> annotationImage;

	Rectangle<float> currentImageRectangle;
	String currentImageId;

	AffineTransform imageToScreen;
	AffineTransform screenToImage;
	float imageLeft, imageTop, imageWidth, imageHeight;
	ScopedPointer<Polytempo_GraphicsPalette> palette;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_GraphicsEditableRegion)
};
