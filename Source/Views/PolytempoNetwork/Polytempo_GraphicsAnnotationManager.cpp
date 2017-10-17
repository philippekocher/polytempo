/*
  ==============================================================================

    Polytempo_GraphicsAnnotationManager.cpp
    Created: 28 Jul 2017 4:27:28pm
    Author:  chris

  ==============================================================================
*/

#include "Polytempo_GraphicsAnnotationManager.h"
#include "Polytempo_GraphicsAnnotationSettingsDialog.h"
#include <string>

juce_ImplementSingleton(Polytempo_GraphicsAnnotationManager)

void Polytempo_GraphicsAnnotationManager::getAnnotationsForImage(String imageId, OwnedArray<Polytempo_GraphicsAnnotation>* pAnnotations) const
{
	pAnnotations->clear();
	for(int iSet = 0; iSet < annotationSets.size(); iSet++)
	{
		if (annotationSets[iSet]->getShow())
		{
			annotationSets[iSet]->getAnnotationsForImage(imageId, pAnnotations);
		}
	}
}

void Polytempo_GraphicsAnnotationManager::createAndAddNewLayer(bool editable)
{
	String filename;
	int index = 1;

	do
	{
		filename = currentDirectory->getFullPathName() + "//" + currentScoreName + "_" + "Layer" + std::to_string(index++) + ".xml";
	} while (File(filename).exists());

	Polytempo_GraphicsAnnotationSet* pSet = new Polytempo_GraphicsAnnotationSet(filename, this);
	pSet->setEdit(editable);
	pSet->SaveToFile();

	annotationSets.add(pSet);
}

void Polytempo_GraphicsAnnotationManager::addAnnotation(Polytempo_GraphicsAnnotation annotation)
{
	if (annotationSets.isEmpty())
	{
		createAndAddNewLayer(true);
	}

	for (int i = 0; i < annotationSets.size(); i++)
	{
		if (annotationSets[i]->getEdit()) {
			annotationSets[i]->addAnnotation(annotation);
			annotationSets[i]->SaveToFile();
		}
	}
}

void Polytempo_GraphicsAnnotationManager::saveAll() const
{
	for (int i = 0; i < annotationSets.size(); i++)
	{
		annotationSets[i]->SaveToFile();
	}
}

bool Polytempo_GraphicsAnnotationManager::getAnchorFlag() const
{
	return showAnchorPoints;
}

void Polytempo_GraphicsAnnotationManager::setAnchorFlag(bool anchorFlag)
{
	showAnchorPoints = anchorFlag;
	sendChangeMessage();
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
		Polytempo_GraphicsAnnotationSet* annotationSet = new Polytempo_GraphicsAnnotationSet(file.getFullPathName(), this);
		if (annotationSet->getScoreName() == scoreName)
			annotationSets.add(annotationSet);
		else
			delete annotationSet;
	}

	sendChangeMessage();
}

void Polytempo_GraphicsAnnotationManager::showSettingsDialog()
{
	Polytempo_GraphicsAnnotationSettingsDialog::show(&annotationSets);
}

void Polytempo_GraphicsAnnotationManager::changeListenerCallback(ChangeBroadcaster*)
{
	sendChangeMessage();
}

bool Polytempo_GraphicsAnnotationManager::removeAnnotation(Uuid id, Polytempo_GraphicsAnnotation* pAnnotation)
{
	for(int iSet = 0; iSet < annotationSets.size(); iSet++)
	{
		if (annotationSets[iSet]->getEdit())
		{
			if (annotationSets[iSet]->removeAnnotation(id, pAnnotation))
			{
				sendChangeMessage();
				return true;
			}
		}
	}

	return false;
}
