/* ==============================================================================
 
 This file is part of the POLYTEMPO software package.
 Copyright (c) 2016 - Zurich University of the Arts,
 ICST Institute for Computer Music and Sound Technology
 http://www.icst.net
 
 Author: Philippe Kocher
 
 POLYTEMPO is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 POLYTEMPO is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this software. If not, see <http://www.gnu.org/licenses/>.
 
 ============================================================================== */

#include "Polytempo_ImageManager.h"
#include "../../Preferences/Polytempo_StoredPreferences.h"
#include "../../Misc/Polytempo_Alerts.h"


Polytempo_ImageManager::~Polytempo_ImageManager()
{
    clearSingletonInstance();
}

juce_ImplementSingleton(Polytempo_ImageManager);

Image* Polytempo_ImageManager::getImage(var imageID)
{
    for(int i=0;i<imageIDs.size();i++)
    {
        if(*imageIDs[i] == imageID.toString())
            return images[i];
    }
    return nullptr;
}

OwnedArray < Image >& Polytempo_ImageManager::getImages()
{
    return images;
}

OwnedArray < String >& Polytempo_ImageManager::getImageIDs()
{
    return imageIDs;
}

String Polytempo_ImageManager::getFileName(var imageID)
{
    for(int i=0;i<loadImageEvents.size();i++)
    {
        if(loadImageEvents[i]->getProperty(eventPropertyString_ImageID) == imageID)
            return loadImageEvents[i]->getProperty(eventPropertyString_URL);
    }

    return String::empty;
}

void Polytempo_ImageManager::deleteAll()
{
    images.clear();
    imageIDs.clear();
    loadImageEvents.clear();
}

void Polytempo_ImageManager::loadImage(Polytempo_Event *event)
{
//    loadImageEvents->set(event->getProperty(eventPropertyString_ImageID), event);
    loadImageEvents.add(event);
    imageIDs.add(new String(event->getProperty(eventPropertyString_ImageID).toString()));
    
    File directory(Polytempo_StoredPreferences::getInstance()->getProps().getValue("scoreFileDirectory"));
    String url(event->getProperty(eventPropertyString_URL).toString());
    
    Image* image = new Image(ImageFileFormat::loadFrom(directory.getChildFile(url)));
    if(*image == Image::null)
    {
        Polytempo_Alert::show("Error", "Can't open file:\n" + directory.getChildFile(url).getFullPathName());
    }
    
    Image* image1 = new Image(image->rescaled(image->getWidth() / 2, image->getHeight() / 2));
    images.add(image1);
    delete image;
}

void Polytempo_ImageManager::replaceImage(var imageID, var url)
{
    File directory(Polytempo_StoredPreferences::getInstance()->getProps().getValue("scoreFileDirectory"));
    Image* image = new Image(ImageFileFormat::loadFrom(directory.getChildFile(url.toString())));

    if(*image == Image::null)
    {
        Polytempo_Alert::show("Error", "Can't open file:\n" + directory.getChildFile(url.toString()).getFullPathName());
    }
    else
    {
        Image* image1 = new Image(image->rescaled(image->getWidth() / 2, image->getHeight() / 2));
        for(int i=0;i<loadImageEvents.size();i++)
        {
            if(loadImageEvents[i]->getProperty(eventPropertyString_ImageID) == imageID)
            {
                loadImageEvents[i]->setProperty(eventPropertyString_URL, url);
                images.set(i, image1);
                break;
            }
        }
    }
    delete image;
}

void Polytempo_ImageManager::eventNotification(Polytempo_Event *event)
{
    if     (event->getType() == eventType_DeleteAll)     deleteAll();
    else if(event->getType() == eventType_LoadImage)     loadImage(event);
}
