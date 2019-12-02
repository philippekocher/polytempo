#ifndef __Polytempo_ImageManager__
#define __Polytempo_ImageManager__

#include "../../Scheduler/Polytempo_EventObserver.h"


class Polytempo_ImageManager : public Polytempo_EventObserver
{
public:
    ~Polytempo_ImageManager();
    
    juce_DeclareSingleton(Polytempo_ImageManager, false);
    
    Image* getImage(var imageID);
    HashMap < var, Image* >& getImages();
    String getFileName(var imageID);
    
    void deleteAll();
    bool loadImage(Polytempo_Event*);
    bool replaceImage(var imageID, String url);
    bool deleteImage(var imageID);
    
    void eventNotification(Polytempo_Event*);
    
private:
    HashMap < var, Polytempo_Event* > loadImageEventMap;
    HashMap < var, Image* > imageMap;
};
    
#endif  // __Polytempo_ImageManager__
