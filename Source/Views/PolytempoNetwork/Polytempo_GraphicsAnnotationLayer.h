#pragma once

#include "JuceHeader.h"
#include "Polytempo_GraphicsViewRegion.h"
#include "Polytempo_GraphicsPalette.h"
#include "Polytempo_GraphicsAnnotation.h"

#define TIMER_ID_REPAINT					1000
#define TIMER_ID_AUTO_ACCEPT				1001
#define MIN_INTERVAL_BETWEEN_REPAINTS_MS	20
#define MIN_MOUSE_DOWN_TIME_MS				500
#define AUTO_ACCEPT_INTERVAL_MS				5000

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
	void handleDeleteSelected();
	void setTemporaryFontSize(float fontSize);
	int getTemporaryFontSize() const;
	void setTemporaryColor(Colour colour);
	void stopAutoAccept();
    void restartAutoAccept();
	Colour getTemporaryColor() const;
    float getTemporaryLineWeight() const;
    void setTemporaryLineWeight(float lineWeight);

private:
	enum Status { Default, FreehandEditing, DragDrop, Selected } status;
    
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
	bool tryGetExistingAnnotation(Point<float> point, Polytempo_GraphicsViewRegion* pRegion);
	void handleStartEditing(Point<int> point);
	void handleFreeHandPainting(const Point<int>& mousePosition);
	void handleEndEdit();
    void setStatus(Status newStatus);

private:
	Atomic <bool> fullUpdateRequired;

	HashMap < String, Polytempo_GraphicsViewRegion* >* pRegionMap;
	std::unique_ptr<Image> annotationImage;
	Polytempo_GraphicsAnnotation temporaryAnnotation;
    Polytempo_GraphicsAnnotation temporaryAnnotationMove;
	Point<int> lastPathPoint;
    int lastAnnotationMode;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_GraphicsAnnotationLayer)
};
