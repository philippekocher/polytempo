/*
  ==============================================================================

    Polytempo_GraphicsAnnotationSet.h
    Created: 29 Jul 2017 2:01:10am
    Author:  chris

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "Polytempo_GraphicsAnnotation.h"

class Polytempo_GraphicsAnnotationSet
{
public:
	Polytempo_GraphicsAnnotationSet(String filename);
	~Polytempo_GraphicsAnnotationSet();

	void getAnnotationsForImage(String imageId, OwnedArray<Polytempo_GraphicsAnnotation>* pAnnotations);
	void addAnnotation(Polytempo_GraphicsAnnotation annotation);

	void SaveToFile();

private:
	void LoadFromFile();
	
	OwnedArray<Polytempo_GraphicsAnnotation> annotations;
	String filename;
	String scoreName;
	String annotationLayerName;
	bool show;
};
