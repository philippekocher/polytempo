#include "Polytempo_ImageManager.h"
#include "../../Preferences/Polytempo_StoredPreferences.h"
#include "../../Misc/Polytempo_Alerts.h"
#ifdef POLYTEMPO_NETWORK
#include "../../Application/PolytempoNetwork/Polytempo_NetworkApplication.h"
#include "../../Application/PolytempoNetwork/Polytempo_NetworkWindow.h"
#endif

Polytempo_ImageManager::~Polytempo_ImageManager()
{
    deleteAll();
    clearSingletonInstance();
}

juce_ImplementSingleton(Polytempo_ImageManager)

Image* Polytempo_ImageManager::getImage(var imageID)
{
    if (imageMap.contains(imageID))
        return imageMap[imageID];

    return nullptr;
}

HashMap<var, Image*>& Polytempo_ImageManager::getImages()
{
    return imageMap;
}

String Polytempo_ImageManager::getFileName(var imageID)
{
    if (imageUrlMap.contains(imageID))
        return imageUrlMap[imageID];

    return String();
}

void Polytempo_ImageManager::deleteAll()
{
    HashMap<var, Image*>::Iterator i(imageMap);
    while (i.next())
    {
        delete i.getValue();
    }

    imageMap.clear();
    imageUrlMap.clear();
}

bool Polytempo_ImageManager::loadImage(var imageID, String url)
{
    if (imageMap.contains(imageID))
    {
        return replaceImage(imageID, url);
    }
    
    File directory(Polytempo_StoredPreferences::getInstance()->getProps().getValue("scoreFileDirectory"));

    if (!File(directory.getChildFile(url)).existsAsFile())
    {
        Polytempo_Alert::show("Error", "Can't open file:\n" + directory.getChildFile(url).getFullPathName());
        return false;
    }

    showLoadStatusWindow(url);
    Image* image = new Image(ImageFileFormat::loadFrom(directory.getChildFile(url)));

    if (*image == Image())
    {
        Polytempo_Alert::show("Error", "Can't open file:\n" + directory.getChildFile(url).getFullPathName());
        delete image;
        MessageManager::callAsync([this]() { hideLoadStatusWindow(); });
        return false;
    }

    delete imageMap.getReference(imageID); // delete the image previously stored under this ID
    imageUrlMap.set(imageID, url);
    imageMap.set(imageID, image);

    MessageManager::callAsync([this]() { hideLoadStatusWindow(); });
    return true;
}

bool Polytempo_ImageManager::replaceImage(var imageID, String url)
{
    if (imageMap.contains(imageID))
    {
        File directory(Polytempo_StoredPreferences::getInstance()->getProps().getValue("scoreFileDirectory"));

        if (!File(directory.getChildFile(url)).existsAsFile())
        {
            Polytempo_Alert::show("Error", "Can't open file:\n" + directory.getChildFile(url).getFullPathName());
            return false;
        }

        showLoadStatusWindow(url);
        Image* image = new Image(ImageFileFormat::loadFrom(directory.getChildFile(url)));

        if (*image == Image())
        {
            Polytempo_Alert::show("Error", "Can't open file:\n" + directory.getChildFile(url).getFullPathName());
            delete image;
            MessageManager::callAsync([this]() { hideLoadStatusWindow(); });
            return false;
        }

        delete imageMap.getReference(imageID); // delete the image previously stored under this ID

        imageUrlMap.set(imageID, url);
        imageMap.set(imageID, image);

        MessageManager::callAsync([this]() { hideLoadStatusWindow(); });
        return true;
    }

    return false;
}

bool Polytempo_ImageManager::deleteImage(var imageID)
{
    if (imageMap.contains(imageID))
    {
        delete imageMap.getReference(imageID); // delete the image stored under this ID
        imageMap.remove(imageID);
        return true;
    }

    return false;
}

void Polytempo_ImageManager::eventNotification(Polytempo_Event* event)
{
    if (event->getType() == eventType_DeleteAll) deleteAll();
    else if (event->getType() == eventType_LoadImage)
    {
        var imageID(event->getProperty(eventPropertyString_ImageID));
        String url(event->getProperty(eventPropertyString_URL).toString());
        MessageManager::callAsync([this, imageID, url]() { loadImage(imageID, url); });
    }
}

void Polytempo_ImageManager::showLoadStatusWindow(var url)
{
    if (loadStatusWindow == nullptr)
        loadStatusWindow = new AlertWindow("Loading...", String(), AlertWindow::NoIcon);
        loadStatusWindow->setMessage(url);
    loadStatusWindow->setVisible(true);
    if (!loadStatusWindow->isCurrentlyModal())
    {
        Polytempo_NetworkApplication* const app = dynamic_cast<Polytempo_NetworkApplication*>(JUCEApplication::getInstance());
        Polytempo_NetworkWindow* window = app->getMainWindow();
        window->getContentComponent()->addChildComponent(loadStatusWindow);
        loadStatusWindow->centreAroundComponent(nullptr,loadStatusWindow->getWidth(), loadStatusWindow->getHeight());
        loadStatusWindow->enterModalState();
    }
}

void Polytempo_ImageManager::hideLoadStatusWindow()
{
    if(loadStatusWindow != nullptr)
    {
        loadStatusWindow->setMessage(String());
        loadStatusWindow->setVisible(false);
        loadStatusWindow->exitModalState(0);
        
        Polytempo_NetworkApplication* const app = dynamic_cast<Polytempo_NetworkApplication*>(JUCEApplication::getInstance());
        Polytempo_NetworkWindow* window = app->getMainWindow();
        window->repaint();
    }
}
