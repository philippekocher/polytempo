/*
  ==============================================================================

    Polytempo_GraphicsAnnotationManager.cpp
    Created: 28 Jul 2017 4:27:28pm
    Author:  chris

  ==============================================================================
*/

#include "Polytempo_GraphicsAnnotationManager.h"

juce_ImplementSingleton(Polytempo_GraphicsAnnotationManager)

void Polytempo_GraphicsAnnotationManager::getAnnotationsForImage(String imageId, OwnedArray<Polytempo_GraphicsAnnotation>* pAnnotations) const
{
	for(int iSet = 0; iSet < annotationSets.size(); iSet++)
	{
		if (annotationSets[iSet]->getShow())
		{
			annotationSets[iSet]->getAnnotationsForImage(imageId, pAnnotations);
		}
	}
}

void Polytempo_GraphicsAnnotationManager::addAnnotation(Polytempo_GraphicsAnnotation annotation)
{
	if (annotationSets.isEmpty())
		annotationSets.add(new Polytempo_GraphicsAnnotationSet(currentDirectory->getFullPathName() + "//" + currentScoreName + "_" + "default.xml"));

	annotationSets[annotationSets.size()-1]->addAnnotation(annotation);
}

void Polytempo_GraphicsAnnotationManager::saveAll() const
{
	for (int i = 0; i < annotationSets.size(); i++)
	{
		annotationSets[i]->SaveToFile();
	}
}

void Polytempo_GraphicsAnnotationManager::initialize(String folder, String scoreName)
{
	annotationSets.clear();

	currentDirectory = new File(folder);
	currentScoreName = scoreName;

	if (!currentDirectory->exists())
		return;

	Array<File> files;
	currentDirectory->findChildFiles(files, File::findFiles, false, "*.xml");

	for(File file : files)
	{
		Polytempo_GraphicsAnnotationSet* annotationSet = new Polytempo_GraphicsAnnotationSet(file.getFullPathName());
		if (annotationSet->getScoreName() == scoreName)
			annotationSets.add(annotationSet);
		else
			delete annotationSet;
	}

	sendChangeMessage();
}
