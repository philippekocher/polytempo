/*
  ==============================================================================

    Polytempo_GraphicsAnnotationManager.cpp
    Created: 28 Jul 2017 4:27:28pm
    Author:  christian.schweizer

  ==============================================================================
*/

#include "Polytempo_GraphicsAnnotationManager.h"
#include "Polytempo_GraphicsAnnotationSettingsDialog.h"
#include <string>

juce_ImplementSingleton(Polytempo_GraphicsAnnotationManager)

void Polytempo_GraphicsAnnotationManager::getAnnotationsForImage(String imageId, OwnedArray<Polytempo_GraphicsAnnotation>* pAnnotations) const
{
	pAnnotations->clear();
	if (annotationMode == Off)
		return;

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
        filename = currentDirectory->getFullPathName() + File::getSeparatorString() + currentScoreName + "_" + "Layer" + std::to_string(index++) + ".xml";
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
	return annotationMode == Edit;
}

void Polytempo_GraphicsAnnotationManager::initialize(String folder, String scoreName)
{
	annotationSets.clear();

	annotationPending = false;
	currentDirectory.reset(new File(folder));
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
		AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Annotation Layers", "More than one editable layers found! Using layer " + highestPriorityEditableLayer->getAnnotationLayerName() + " only");
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

bool Polytempo_GraphicsAnnotationManager::getAnnotation(Uuid id, Polytempo_GraphicsAnnotation* pAnnotation)
{
	for(int iSet = 0; iSet < annotationSets.size(); iSet++)
	{
		if (annotationSets[iSet]->getEdit())
		{
			if (annotationSets[iSet]->getAnnotation(id, pAnnotation))
			{
				annotationSets[iSet]->SaveToFile();
				sendChangeMessage();
				return true;
			}
		}
	}

	return false;
}

bool Polytempo_GraphicsAnnotationManager::removeAnnotation(Uuid id)
{
	for (int iSet = 0; iSet < annotationSets.size(); iSet++)
	{
		if (annotationSets[iSet]->getEdit())
		{
			if (annotationSets[iSet]->removeAnnotation(id))
			{
				annotationSets[iSet]->SaveToFile();
				sendChangeMessage();
				return true;
			}
		}
	}

	return false;
}

bool Polytempo_GraphicsAnnotationManager::isAnnotationPending() const
{
	csPendingAnnotation.enter();
	bool ret = annotationPending;
	csPendingAnnotation.exit();
	return ret;
}

bool Polytempo_GraphicsAnnotationManager::trySetAnnotationPending(Polytempo_GraphicsAnnotationLayer* pEditableRegion)
{
	bool ret;

	csPendingAnnotation.enter();
	if (annotationPending)
		ret = false;
	else {
		annotationPending = true;
		pCurrentPendingAnnotationLyer = pEditableRegion;
		ret = true;
	}
	csPendingAnnotation.exit();
	return ret;
}

void Polytempo_GraphicsAnnotationManager::resetAnnotationPending(Polytempo_GraphicsAnnotationLayer* pEditableRegion)
{
	csPendingAnnotation.enter();
	if (pEditableRegion == pCurrentPendingAnnotationLyer)
	{
		annotationPending = false;
		pCurrentPendingAnnotationLyer = nullptr;
	}
	csPendingAnnotation.exit();
}

Polytempo_GraphicsAnnotationLayer* Polytempo_GraphicsAnnotationManager::getCurrentPendingAnnotationLayer() const
{
	return pCurrentPendingAnnotationLyer;
}

void Polytempo_GraphicsAnnotationManager::setAnnotationMode(eAnnotationMode mode)
{
	annotationMode = mode;
	sendChangeMessage();
}

Polytempo_GraphicsAnnotationManager::eAnnotationMode Polytempo_GraphicsAnnotationManager::getAnnotationMode() const
{
	return annotationMode;
}
