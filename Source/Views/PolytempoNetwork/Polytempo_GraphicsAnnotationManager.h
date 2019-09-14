/*
  ==============================================================================

    Polytempo_GraphicsAnnotationManager.h
    Created: 28 Jul 2017 4:27:28pm
    Author:  christian.schweizer

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "Polytempo_GraphicsAnnotation.h"
#include "Polytempo_GraphicsAnnotationSet.h"
#include "Polytempo_GraphicsAnnotationLayer.h"

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
	bool getAnnotation(Uuid id, Polytempo_GraphicsAnnotation* pAnnotation);
	bool removeAnnotation(Uuid id);
	bool isAnnotationPending() const;
	bool trySetAnnotationPending(Polytempo_GraphicsAnnotationLayer* pEditableRegion);
	void resetAnnotationPending(Polytempo_GraphicsAnnotationLayer* pEditableRegion);
	Polytempo_GraphicsAnnotationLayer* getCurrentPendingAnnotationLayer() const;

private:
	Polytempo_GraphicsAnnotationManager() : showAnchorPoints(false), annotationPending(false), pCurrentPendingAnnotationLyer(nullptr)
	{
	}

private:
	OwnedArray<Polytempo_GraphicsAnnotationSet> annotationSets;
	std::unique_ptr<File> currentDirectory;
	String currentScoreName;
	bool showAnchorPoints;
	bool annotationPending;
	CriticalSection csPendingAnnotation;
	Polytempo_GraphicsAnnotationLayer* pCurrentPendingAnnotationLyer;
};
