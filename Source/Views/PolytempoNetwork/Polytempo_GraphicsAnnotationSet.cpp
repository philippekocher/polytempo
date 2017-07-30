/*
  ==============================================================================

    Polytempo_GraphicsAnnotationSet.cpp
    Created: 29 Jul 2017 2:01:10am
    Author:  chris

  ==============================================================================
*/

#include "Polytempo_GraphicsAnnotationSet.h"

Polytempo_GraphicsAnnotationSet::Polytempo_GraphicsAnnotationSet(String filename) : filename(filename)
{
	LoadFromFile();
}

Polytempo_GraphicsAnnotationSet::~Polytempo_GraphicsAnnotationSet()
{
}

void Polytempo_GraphicsAnnotationSet::getAnnotationsForImage(String imageId, OwnedArray<Polytempo_GraphicsAnnotation>* pAnnotations)
{
	for (Polytempo_GraphicsAnnotation* annotation : annotations)
	{
		if (annotation->imageId == imageId)
			pAnnotations->add(new Polytempo_GraphicsAnnotation(*annotation));
	}
}

void Polytempo_GraphicsAnnotationSet::addAnnotation(Polytempo_GraphicsAnnotation annotation)
{
	annotations.add(new Polytempo_GraphicsAnnotation(annotation));
}

void Polytempo_GraphicsAnnotationSet::LoadFromFile()
{
	File file = File(filename);
	if (!file.exists())
		return;

	ScopedPointer<XmlDocument> xmlDoc = new XmlDocument(file);
	ScopedPointer<XmlElement> root = xmlDoc->getDocumentElement();
	if (root->getTagName() != XML_TAG_ROOT)
		return;
		
	if (!root->hasAttribute(XML_ATTRIBUTE_SCORENAME) || !root->hasAttribute(XML_ATTRIBUTE_LAYERNAME) || !root->hasAttribute(XML_ATTRIBUTE_SHOW))
		return;
		
	scoreName = root->getStringAttribute("ScoreName");
	annotationLayerName = root->getStringAttribute("AnnotationLayerName");
	show = root->getBoolAttribute("Show");

	XmlElement* xmlAnnotationList = root->getChildByName("Annotations");
	if (xmlAnnotationList == nullptr)
		return;

	XmlElement* xmlAnnotation = xmlAnnotationList->getChildByName("Annotation");
	while (xmlAnnotation != nullptr)
	{
		Path path;
		path.restoreFromString(xmlAnnotation->getStringAttribute("Path"));

		annotations.add(new Polytempo_GraphicsAnnotation(
			xmlAnnotation->getStringAttribute("ImageId"),
			Point<float>(
				float(xmlAnnotation->getDoubleAttribute("ReferencePointX")), 
				float(xmlAnnotation->getDoubleAttribute("ReferencePointY"))),
			Colour::fromString(xmlAnnotation->getStringAttribute("Color")),
			path,
			xmlAnnotation->getStringAttribute("Text")));

		xmlAnnotation = xmlAnnotation->getNextElement();
	}
}

void Polytempo_GraphicsAnnotationSet::SaveToFile()
{
	File file = File(filename);
	ScopedPointer<XmlElement> xmlMain = new XmlElement(XML_TAG_ROOT);
	xmlMain->setAttribute(XML_ATTRIBUTE_SCORENAME, scoreName);
	xmlMain->setAttribute(XML_ATTRIBUTE_LAYERNAME, annotationLayerName);
	xmlMain->setAttribute(XML_ATTRIBUTE_SHOW, show);

	XmlElement* xmlAnnotations = new XmlElement(XML_TAG_ANNOTATIONS);
	for (Polytempo_GraphicsAnnotation* annotation : annotations)
	{
		XmlElement* xmlAnnotation = new XmlElement(XML_TAG_ANNOTATION);
		xmlAnnotation->setAttribute(XML_ATTRIBUTE_IMAGEID, annotation->imageId);
		xmlAnnotation->setAttribute(XML_ATTRIBUTE_REFERENCEX, annotation->referencePoint.getX());
		xmlAnnotation->setAttribute(XML_ATTRIBUTE_REFERENCEY, annotation->referencePoint.getY());
		xmlAnnotation->setAttribute(XML_ATTRIBUTE_TEXT, annotation->text);
		xmlAnnotation->setAttribute(XML_ATTRIBUTE_PATH, annotation->freeHandPath.toString());
		xmlAnnotation->setAttribute(XML_ATTRIBUTE_COLOR, annotation->color.toString());
		xmlAnnotations->addChildElement(xmlAnnotation);
	}
	xmlMain->addChildElement(xmlAnnotations);

	String xmlDocStr = xmlMain->createDocument("");
	file.deleteFile();
	file.create();
	file.appendText(xmlDocStr.toWideCharPointer());

	xmlMain->deleteAllChildElements();
}

String Polytempo_GraphicsAnnotationSet::getScoreName() const
{
	return scoreName;
}

String Polytempo_GraphicsAnnotationSet::getAnnotationLayerName() const
{
	return annotationLayerName;
}

bool Polytempo_GraphicsAnnotationSet::getShow() const
{
	return show;
}
