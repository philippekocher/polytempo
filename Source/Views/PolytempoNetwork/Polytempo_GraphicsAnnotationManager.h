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

class Polytempo_GraphicsAnnotationManager : public ChangeBroadcaster
{
public:
	juce_DeclareSingleton(Polytempo_GraphicsAnnotationManager, false)

	void getAnnotationsForImage(String imageId, OwnedArray<Polytempo_GraphicsAnnotation>* pAnnotations) const;
	void addAnnotation(Polytempo_GraphicsAnnotation annotation);
	void saveAll() const;
	void initialize(String folder, String scoreName);
	void showSettingsDialog();

private:
	OwnedArray<Polytempo_GraphicsAnnotationSet> annotationSets;
	ScopedPointer<File> currentDirectory;
	String currentScoreName;
};
