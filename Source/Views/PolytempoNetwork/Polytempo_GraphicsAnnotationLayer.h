/*
  ==============================================================================

    Polytempo_GraphicsAnnotationLayer.h
    Created: 22 Jul 2018 5:42:33am
    Author:  chris

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "Polytempo_GraphicsViewRegion.h"

//==============================================================================
/*
*/
class Polytempo_GraphicsAnnotationLayer    : public Component, MultiTimer, KeyListener, ChangeListener
{
public:
    Polytempo_GraphicsAnnotationLayer(HashMap < String, Polytempo_GraphicsViewRegion* >* pRegionMap);
    ~Polytempo_GraphicsAnnotationLayer();
	
	void requireUpdate();
	void handleEndEditAccept();
	void handleEndEditCancel();
	void setTemporaryFontSize(float fontSize);
	int getTemporaryFontSize() const;
	void setTemporaryColor(Colour colour);
	void stopAutoAccept();
	void hitBtnColor() const;
	void hitBtnTextSize() const;

private:
	void mouseDown(const MouseEvent& event) override;
	void mouseUp(const MouseEvent& event) override;
	void mouseDrag(const MouseEvent& event) override;
	void mouseDoubleClick(const MouseEvent& event) override; 
	bool keyPressed(const KeyPress& key, Component* /*originatingComponent*/) override;
	void timerCallback(int timerID) override;
	void changeListenerCallback(ChangeBroadcaster* source) override; 
	void paint(Graphics&) override;
	void resized() override;

	Polytempo_GraphicsViewRegion* getRegionAt(Point<int> point) const;
	static void paintAnnotation(Graphics& g, const Polytempo_GraphicsAnnotation* annotation, bool anchorFlag, Colour anchorColor);
	void prepareAnnotationLayer();
	bool TryGetExistingAnnotation(Point<int> point);
	void handleStartEditing(Point<int> point);
	void handleFreeHandPainting(const Point<int>& mousePosition);
	void handleEndEdit();

private:
	enum Status { Default, FreehandEditing } status;
	Atomic <bool> fullUpdateRequired;

	HashMap < String, Polytempo_GraphicsViewRegion* >* pRegionMap;
	ScopedPointer<Image> annotationImage;
	ScopedPointer<Polytempo_GraphicsPalette> palette;
	Polytempo_GraphicsAnnotation temporaryAnnotation;
	Point<int> lastPathPoint;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_GraphicsAnnotationLayer)
};
