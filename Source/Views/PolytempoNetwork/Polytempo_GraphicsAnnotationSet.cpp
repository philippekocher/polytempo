/*
  ==============================================================================

    Polytempo_GraphicsAnnotationSet.cpp
    Created: 29 Jul 2017 2:01:10am
    Author:  chris

  ==============================================================================
*/

#include "Polytempo_GraphicsAnnotationSet.h"

Polytempo_GraphicsAnnotationSet::Polytempo_GraphicsAnnotationSet(String filename)
{
	File f = File(filename);
	filePath = f.getParentDirectory().getFullPathName();

	String fn = f.getFileNameWithoutExtension();
	int index = fn.lastIndexOf("_");
	if (index < 0 || index >= fn.length()-1)
		return;
	
	scoreName = fn.substring(0, index);
	annotationLayerName = fn.substring(index + 1);

	loadFromFile();
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

void Polytempo_GraphicsAnnotationSet::loadFromFile()
{
	File file = File(getFileName());
	if (!file.exists())
		return;

	ScopedPointer<XmlDocument> xmlDoc = new XmlDocument(file);
	ScopedPointer<XmlElement> root = xmlDoc->getDocumentElement();
	if (root->getTagName() != XML_TAG_ROOT)
		return;
		
	if (!root->hasAttribute(XML_ATTRIBUTE_SCORENAME) || !root->hasAttribute(XML_ATTRIBUTE_LAYERNAME) || !root->hasAttribute(XML_ATTRIBUTE_SHOW))
		return;
		
	scoreName = root->getStringAttribute(XML_ATTRIBUTE_SCORENAME);
	annotationLayerName = root->getStringAttribute(XML_ATTRIBUTE_LAYERNAME);
	show = root->getBoolAttribute(XML_ATTRIBUTE_SHOW);

	XmlElement* xmlAnnotationList = root->getChildByName(XML_TAG_ANNOTATIONS);
	if (xmlAnnotationList == nullptr)
		return;

	XmlElement* xmlAnnotation = xmlAnnotationList->getChildByName(XML_TAG_ANNOTATION);
	while (xmlAnnotation != nullptr)
	{
		Path path;
		path.restoreFromString(xmlAnnotation->getStringAttribute(XML_ATTRIBUTE_PATH));

		annotations.add(new Polytempo_GraphicsAnnotation(
			xmlAnnotation->getStringAttribute(XML_ATTRIBUTE_IMAGEID),
			Point<float>(
				float(xmlAnnotation->getDoubleAttribute(XML_ATTRIBUTE_REFERENCEX)), 
				float(xmlAnnotation->getDoubleAttribute(XML_ATTRIBUTE_REFERENCEY))),
			Colour::fromString(xmlAnnotation->getStringAttribute(XML_ATTRIBUTE_COLOR)),
			path,
			xmlAnnotation->getStringAttribute(XML_ATTRIBUTE_TEXT),
			xmlAnnotation->getIntAttribute(XML_ATTRIBUTE_FONTSIZE, 14)));

		xmlAnnotation = xmlAnnotation->getNextElement();
	}
}

String Polytempo_GraphicsAnnotationSet::getFileName(String newLayerName) const
{
	return filePath + "\\" + scoreName + "_" + (newLayerName.isEmpty()?annotationLayerName:newLayerName) + ".xml";
}

bool Polytempo_GraphicsAnnotationSet::SaveToFile()
{
	File file = File(getFileName());
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
		xmlAnnotation->setAttribute(XML_ATTRIBUTE_FONTSIZE, annotation->fontSize);
		xmlAnnotations->addChildElement(xmlAnnotation);
	}
	xmlMain->addChildElement(xmlAnnotations);

	String xmlDocStr = xmlMain->createDocument("");
	file.deleteFile();
	Result result = file.create();
	if (result.failed())
	{
		xmlMain->deleteAllChildElements();
		return false;
	}

	bool ok = file.appendText(xmlDocStr.toWideCharPointer());
	if (!ok)
	{
		xmlMain->deleteAllChildElements();
		return false;
	}

	xmlMain->deleteAllChildElements();
	return true;
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

bool Polytempo_GraphicsAnnotationSet::setAnnotationLayerName(String newLayerName)
{
	if (newLayerName.contains("_"))
		return false;

	String originalFileName = getFileName();
	String newFileName = getFileName(newLayerName);
	if(File(newFileName).exists())
	{
		return false;
	}

	annotationLayerName = newLayerName;
	bool ok = SaveToFile();
	if (!ok)
	{
		return false;
	}

	File(originalFileName).deleteFile();
	return true;
}
