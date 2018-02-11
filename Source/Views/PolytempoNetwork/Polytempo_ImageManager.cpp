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
    deleteAll();
    clearSingletonInstance();
}

juce_ImplementSingleton(Polytempo_ImageManager);

Image* Polytempo_ImageManager::getImage(var imageID)
{
    if(imageMap.contains(imageID)) return imageMap[imageID];
    else return nullptr;
}

HashMap < var, Image* >& Polytempo_ImageManager::getImages()
{
    return imageMap;
}

String Polytempo_ImageManager::getFileName(var imageID)
{
    if(loadImageEventMap.contains(imageID)) return loadImageEventMap[imageID]->getProperty(eventPropertyString_URL);
    else return String::empty;
}

void Polytempo_ImageManager::deleteAll()
{
    HashMap < var, Image* >::Iterator i (imageMap);
    while(i.next())
    {
        delete i.getValue();
    }
    
    imageMap.clear();
    loadImageEventMap.clear();
}

bool Polytempo_ImageManager::loadImage(Polytempo_Event *event)
{
    var imageID = event->getProperty(eventPropertyString_ImageID);
    String url(event->getProperty(eventPropertyString_URL).toString());
    File directory(Polytempo_StoredPreferences::getInstance()->getProps().getValue("scoreFileDirectory"));

    if(!File(directory.getChildFile(url)).existsAsFile())
    {
        Polytempo_Alert::show("Error", "Can't open file:\n" + directory.getChildFile(url).getFullPathName());
        return false;
    }
   
    Image* image = new Image(ImageFileFormat::loadFrom(directory.getChildFile(url)));
    
    if(*image == Image::null)
    {
        Polytempo_Alert::show("Error", "Can't open file:\n" + directory.getChildFile(url).getFullPathName());
        delete image;
        return false;
    }
    else
    {
        delete imageMap.getReference(imageID); // delete the image previously stored under this ID
        
        loadImageEventMap.set(imageID, event);
        imageMap.set(imageID, image);

        return true;
    }
    return false;
}

bool Polytempo_ImageManager::replaceImage(var imageID, String url)
{
    if(loadImageEventMap.contains(imageID))
    {
        File directory(Polytempo_StoredPreferences::getInstance()->getProps().getValue("scoreFileDirectory"));

        if(!File(directory.getChildFile(url)).existsAsFile())
        {
            Polytempo_Alert::show("Error", "Can't open file:\n" + directory.getChildFile(url).getFullPathName());
            return false;
        }

        Image* image = new Image(ImageFileFormat::loadFrom(directory.getChildFile(url)));

        if(*image == Image::null)
        {
            Polytempo_Alert::show("Error", "Can't open file:\n" + directory.getChildFile(url).getFullPathName());
            delete image;
            return false;
        }
        else
        {
            delete imageMap.getReference(imageID); // delete the image previously stored under this ID
        
            Polytempo_Event *event = loadImageEventMap[imageID];
            event->setProperty(eventPropertyString_URL, url);

            imageMap.set(imageID, image);
  
            return true;
        }
    }
    return false;
}

bool Polytempo_ImageManager::deleteImage(var imageID)
{
    if(imageMap.contains(imageID))
    {
        delete imageMap.getReference(imageID); // delete the image stored under this ID
        imageMap.remove(imageID);        
        return true;
    }
    else return false;
}


void Polytempo_ImageManager::eventNotification(Polytempo_Event *event)
{
    if     (event->getType() == eventType_DeleteAll)     deleteAll();
    else if(event->getType() == eventType_LoadImage)     loadImage(event);
}
