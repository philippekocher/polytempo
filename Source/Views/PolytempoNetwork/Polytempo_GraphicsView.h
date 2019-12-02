#pragma once

#include "Polytempo_GraphicsViewRegion.h"
#include "../../Scheduler/Polytempo_EventObserver.h"
#include "Polytempo_GraphicsAnnotationLayer.h"

class Polytempo_GraphicsView : public Component,
                               public Polytempo_EventObserver
{
public:
    Polytempo_GraphicsView ();
    ~Polytempo_GraphicsView ();

    void paint(Graphics& g);
    void resized();
    void eventNotification(Polytempo_Event *event);
    
private:
    void deleteAll();
    void clearAll();
    
    void addRegion(Polytempo_Event *event);
    void addSection(Polytempo_Event *event);
    void displayImage(Polytempo_Event *event);
    void displayText(Polytempo_Event *event);
    void displayProgessbar(Polytempo_Event *event);

    HashMap < String, Polytempo_GraphicsViewRegion* > regionsMap;
    HashMap < String, var > sectionBoundsMap;
    HashMap < String, String > sectionImageIDMap;
    
    std::unique_ptr < Polytempo_Progressbar > progressbar;
	std::unique_ptr < Polytempo_GraphicsAnnotationLayer > annotationLayer;
};