#pragma once

#include "../../Scheduler/Polytempo_EventObserver.h"

class Polytempo_ImageManager : public Polytempo_EventObserver
{
public:
    ~Polytempo_ImageManager() override;

    juce_DeclareSingleton(Polytempo_ImageManager, false)

    Image* getImage(var imageID);
    HashMap<var, Image*>& getImages();
    String getFileName(var imageID);

    void deleteAll();
    bool loadImage(var imageID, String url);
    bool replaceImage(var imageID, String url);
    bool deleteImage(var imageID);

    void eventNotification(Polytempo_Event*) override;

private:
    HashMap<var, String> imageUrlMap;
    HashMap<var, Image*> imageMap;

    AlertWindow* loadStatusWindow = nullptr;
    void showLoadStatusWindow(var url);
    void hideLoadStatusWindow();
};
