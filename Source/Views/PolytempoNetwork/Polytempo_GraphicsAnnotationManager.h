/*
  ==============================================================================

    Polytempo_GraphicsAnnotationManager.h
    Created: 28 Jul 2017 4:27:28pm
    Author:  chris

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "Polytempo_GraphicsAnnotation.h"
#include "Polytempo_GraphicsAnnotationSet.h"
#include "Polytempo_GraphicsEditableRegion.h"

class Polytempo_GraphicsAnnotationManager : public ChangeBroadcaster, public ChangeListener
{
public:
	juce_DeclareSingleton(Polytempo_GraphicsAnnotationManager, false)

	void initialize(String folder, String scoreName);
	void showSettingsDialog();

	void getAnnotationsForImage(String imageId, OwnedArray<Polytempo_GraphicsAnnotation>* pAnnotations) const;
	void createAndAddNewLayer(bool editable);
	void addAnnotation(Polytempo_GraphicsAnnotation annotation);
	void saveAll() const;
	bool getAnchorFlag() const;
	void setAnchorFlag(bool anchorFlag);

	void changeListenerCallback(ChangeBroadcaster*) override;
	bool removeAnnotation(Uuid id, Polytempo_GraphicsAnnotation* pAnnotation);
	bool isAnnotationPending();
	bool trySetAnnotationPending(Polytempo_GraphicsEditableRegion* pEditableRegion);
	void resetAnnotationPending(Polytempo_GraphicsEditableRegion* pEditableRegion);
	Polytempo_GraphicsEditableRegion* getCurrentPendingAnnotationRegion() const;

private:
	Polytempo_GraphicsAnnotationManager() : showAnchorPoints(false), annotationPending(false), pCurrentPendingEditableRegion(nullptr)
	{
	};

private:
	OwnedArray<Polytempo_GraphicsAnnotationSet> annotationSets;
	ScopedPointer<File> currentDirectory;
	String currentScoreName;
	bool showAnchorPoints;
	bool annotationPending;
	CriticalSection csPendingAnnotation;
	Polytempo_GraphicsEditableRegion* pCurrentPendingEditableRegion;
};
