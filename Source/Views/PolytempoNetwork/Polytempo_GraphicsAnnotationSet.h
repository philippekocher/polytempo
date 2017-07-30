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

#define XML_TAG_ROOT				"AnnotationSet"
#define XML_TAG_ANNOTATIONS			"Annotations"
#define XML_TAG_ANNOTATION			"Annotation"
#define XML_ATTRIBUTE_SCORENAME		"ScoreName"
#define XML_ATTRIBUTE_LAYERNAME		"AnnotationLayerName"
#define XML_ATTRIBUTE_SHOW			"Show"
#define XML_ATTRIBUTE_IMAGEID		"ImageId"
#define XML_ATTRIBUTE_REFERENCEX	"ReferencePointX"
#define XML_ATTRIBUTE_REFERENCEY	"ReferencePointY"
#define XML_ATTRIBUTE_TEXT			"Text"
#define XML_ATTRIBUTE_PATH			"Path"
#define XML_ATTRIBUTE_COLOR			"Color"


class Polytempo_GraphicsAnnotationSet
{
public:
	Polytempo_GraphicsAnnotationSet(String filename);
	~Polytempo_GraphicsAnnotationSet();

	void getAnnotationsForImage(String imageId, OwnedArray<Polytempo_GraphicsAnnotation>* pAnnotations);
	void addAnnotation(Polytempo_GraphicsAnnotation annotation);

	void SaveToFile();
	String getScoreName() const;
	String getAnnotationLayerName() const;
	bool getShow() const;

private:
	void LoadFromFile();
	
	OwnedArray<Polytempo_GraphicsAnnotation> annotations;
	String filename;
	String scoreName;
	String annotationLayerName;
	bool show;
};
