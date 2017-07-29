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
		annotationSets[iSet]->getAnnotationsForImage(imageId, pAnnotations);
	}
}

void Polytempo_GraphicsAnnotationManager::addAnnotation(Polytempo_GraphicsAnnotation annotation)
{
	if (annotationSets.isEmpty())
		annotationSets.add(new Polytempo_GraphicsAnnotationSet("C:\\temp\\testAnnotations.xml"));	// todo: remove

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
	annotationSets.add(new Polytempo_GraphicsAnnotationSet("C:\\temp\\testAnnotations.xml"));
}
