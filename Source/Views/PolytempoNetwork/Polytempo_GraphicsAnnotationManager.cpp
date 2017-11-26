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
		filename = currentDirectory->getFullPathName() + "\\" + currentScoreName + "_" + "Layer" + std::to_string(index++) + ".xml";
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

	sendChangeMessage();
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

	annotationPending = false;
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

	// check editable layers
	Polytempo_GraphicsAnnotationSet* highestPriorityEditableLayer = nullptr;
	bool multipleEditableLayers = false;
	for (Polytempo_GraphicsAnnotationSet* annotationSet : annotationSets)
	{
		if(annotationSet->getEdit())
		{
			if (highestPriorityEditableLayer == nullptr)
				highestPriorityEditableLayer = annotationSet;
			else
			{
				multipleEditableLayers = true;
				if(annotationSet->getAnnotationLayerName() < highestPriorityEditableLayer->getAnnotationLayerName())
				{
					highestPriorityEditableLayer->setEdit(false);
					highestPriorityEditableLayer->SaveToFile();
					highestPriorityEditableLayer = annotationSet;
				}
				else
				{
					annotationSet->setEdit(false);
					annotationSet->SaveToFile();
				}
			}
		}
	}
	// make sure editable layer is visible
	if(highestPriorityEditableLayer!=nullptr)
	{
		highestPriorityEditableLayer->setShow(true);
		highestPriorityEditableLayer->SaveToFile();
	}
	if(multipleEditableLayers)
	{
		AlertWindow::showMessageBox(AlertWindow::WarningIcon, "Annotation Layers", "More than one editable layers found! Using layer " + highestPriorityEditableLayer->getAnnotationLayerName() + " only");
	}
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
				annotationSets[iSet]->SaveToFile();
				sendChangeMessage();
				return true;
			}
		}
	}

	return false;
}

bool Polytempo_GraphicsAnnotationManager::isAnnotationPending()
{
	csPendingAnnotation.enter();
	bool ret = annotationPending;
	csPendingAnnotation.exit();
	return ret;
}

bool Polytempo_GraphicsAnnotationManager::trySetAnnotationPending(Polytempo_GraphicsEditableRegion* pEditableRegion)
{
	bool ret;

	csPendingAnnotation.enter();
	if (annotationPending)
		ret = false;
	else {
		annotationPending = true;
		pCurrentPendingEditableRegion = pEditableRegion;
		ret = true;
	}
	csPendingAnnotation.exit();
	return ret;
}

void Polytempo_GraphicsAnnotationManager::resetAnnotationPending(Polytempo_GraphicsEditableRegion* pEditableRegion)
{
	csPendingAnnotation.enter();
	if (pEditableRegion == pCurrentPendingEditableRegion)
	{
		annotationPending = false;
		pCurrentPendingEditableRegion = nullptr;
	}
	csPendingAnnotation.exit();
}

Polytempo_GraphicsEditableRegion* Polytempo_GraphicsAnnotationManager::getCurrentPendingAnnotationRegion() const
{
	return pCurrentPendingEditableRegion;
}
