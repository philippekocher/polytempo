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
	scoreName = root->getStringAttribute("ScoreName");
	annotationLayerName = root->getStringAttribute("AnnotationLayerName");
	show = root->getBoolAttribute("Show");

	XmlElement* xmlAnnotation = root->getChildByName("Annotations")->getChildByName("Annotation");
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
	ScopedPointer<XmlElement> xmlMain = new XmlElement("AnnotationSet");
	xmlMain->setAttribute("ScoreName", scoreName);
	xmlMain->setAttribute("AnnotationLayerName", annotationLayerName);
	xmlMain->setAttribute("Show", show);

	XmlElement* xmlAnnotations = new XmlElement("Annotations");
	for (Polytempo_GraphicsAnnotation* annotation : annotations)
	{
		XmlElement* xmlAnnotation = new XmlElement("Annotation");
		xmlAnnotation->setAttribute("ImageId", annotation->imageId);
		xmlAnnotation->setAttribute("ReferencePointX", annotation->referencePoint.getX());
		xmlAnnotation->setAttribute("ReferencePointY", annotation->referencePoint.getY());
		xmlAnnotation->setAttribute("Text", annotation->text);
		xmlAnnotation->setAttribute("Path", annotation->freeHandPath.toString());
		xmlAnnotation->setAttribute("Color", annotation->color.toString());
		xmlAnnotations->addChildElement(xmlAnnotation);
	}
	xmlMain->addChildElement(xmlAnnotations);

	String xmlDocStr = xmlMain->createDocument("");
	file.deleteFile();
	file.create();
	file.appendText(xmlDocStr.toWideCharPointer());

	xmlMain->deleteAllChildElements();
}
