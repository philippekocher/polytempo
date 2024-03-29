#include "Polytempo_GraphicsView.h"
#include "Polytempo_ImageManager.h"
#include "../../Scheduler/Polytempo_ScoreScheduler.h"
#include "../../Preferences/Polytempo_StoredPreferences.h"

Polytempo_GraphicsView::Polytempo_GraphicsView()
{
    setOpaque(true);
    annotationLayer.reset(new Polytempo_GraphicsAnnotationLayer(&regionsMap));
    annotationLayer->setVisible(true);
    addChildComponent(annotationLayer.get());
}

Polytempo_GraphicsView::~Polytempo_GraphicsView()
{
    deleteAll();
    annotationLayer = nullptr;
}

void Polytempo_GraphicsView::paint(Graphics& g)
{
    g.fillAll(Colours::white);
}

void Polytempo_GraphicsView::resized()
{
    HashMap<String, Polytempo_GraphicsViewRegion*>::Iterator it1(regionsMap);
    while (it1.next()) { it1.getValue()->resized(); }

    annotationLayer->setBounds(getLocalBounds());
}

void Polytempo_GraphicsView::eventNotification(Polytempo_Event* event)
{
    if (event->getType() == eventType_DeleteAll) deleteAll();
    else if (event->getType() == eventType_ClearAll) clearAll();
    else if (event->getType() == eventType_AddRegion) addRegion(event);
    else if (event->getType() == eventType_AddSection) addSection(event);
    else if (event->getType() == eventType_Image) displayImage(event);
    else if (event->getType() == eventType_AppendImage) displayImage(event);
    else if (event->getType() == eventType_Text) displayText(event);
    else if (event->getType() == eventType_Progressbar) displayProgessbar(event);
}

void Polytempo_GraphicsView::deleteAll()
{
    HashMap<String, Polytempo_GraphicsViewRegion*>::Iterator it(regionsMap);
    while (it.next())
    {
        removeChildComponent(it.getValue());
        delete(it.getValue());
    }
    regionsMap.clear();

    sectionBoundsMap.clear();
    sectionImageIDMap.clear();
}

void Polytempo_GraphicsView::clearAll()
{
    annotationLayer->requireUpdate();
}

void Polytempo_GraphicsView::addRegion(Polytempo_Event* event)
{
    const MessageManagerLock mml(Thread::getCurrentThread());

    Polytempo_GraphicsViewRegion* region = new Polytempo_GraphicsViewRegion(event->getProperty(eventPropertyString_RegionID));
    addChildComponent(region);

    delete regionsMap[event->getProperty(eventPropertyString_RegionID)]; // old region
    regionsMap.set(event->getProperty(eventPropertyString_RegionID), region);

    Array<var> r;
    if(event->hasProperty(eventPropertyString_Rect))
        r = *event->getProperty(eventPropertyString_Rect).getArray();
    else
        r = Polytempo_Event::defaultRectangle();

    region->setRelativeBounds(Rectangle<float>(r[0], r[1], r[2], r[3]));

    region->setMaxImageZoom(event->getProperty(eventPropertyString_MaxZoom));
    
    region->setLayout(event->getProperty(eventPropertyString_Layout));
    region->setAlignment(event->getProperty(eventPropertyString_XAlignment), event->getProperty(eventPropertyString_YAlignment));

    region->repaint();
    annotationLayer->requireUpdate();
}

void Polytempo_GraphicsView::addSection(Polytempo_Event* event)
{
    sectionBoundsMap.set(event->getProperty(eventPropertyString_SectionID), event->getProperty(eventPropertyString_Rect));
    sectionImageIDMap.set(event->getProperty(eventPropertyString_SectionID), event->getProperty(eventPropertyString_ImageID));
}

void Polytempo_GraphicsView::displayImage(Polytempo_Event* event)
{
    if (!regionsMap.contains(event->getProperty(eventPropertyString_RegionID)))
        return; // invalid region id

    annotationLayer->requireUpdate();

    Polytempo_GraphicsViewRegion* region = regionsMap[event->getProperty(eventPropertyString_RegionID)];
    Image* image = nullptr;
    var rect;
    var imageId;

    if (!event->getProperty(eventPropertyString_SectionID).isVoid()) // a section ID is given
    {
        imageId = sectionImageIDMap[event->getProperty(eventPropertyString_SectionID)];
        image = Polytempo_ImageManager::getInstance()->getImage(imageId);
        rect = sectionBoundsMap[event->getProperty(eventPropertyString_SectionID)];
    }
    else if (!event->getProperty(eventPropertyString_ImageID).isVoid()) // a image ID is given
    {
        imageId = event->getProperty(eventPropertyString_ImageID);
        image = Polytempo_ImageManager::getInstance()->getImage(imageId);
        rect = event->getProperty(eventPropertyString_Rect);
    }
    else
    {
        region->clear();
        return;
    }

    if (rect == var()) rect = Polytempo_Event::defaultRectangle();

    region->setImage(image, rect, imageId, event->getType() == eventType_AppendImage);
    MessageManager::callAsync([region]() { region->repaint(); });
}

void Polytempo_GraphicsView::displayText(Polytempo_Event* event)
{
    if (!regionsMap.contains(event->getProperty(eventPropertyString_RegionID)))
        return; // invalid region id

    annotationLayer->requireUpdate();

    Polytempo_GraphicsViewRegion* region = regionsMap[event->getProperty(eventPropertyString_RegionID)];

    if (event->getProperty("value").isVoid()) // no text given
    {
        region->clear();
        return;
    }

    if (event->getValue().isDouble())
        region->setText(String((double)event->getProperty("value"),0));
    else
        region->setText(String(event->getProperty("value").toString()));

    MessageManager::callAsync([region]() { region->repaint(); });
}

void Polytempo_GraphicsView::displayProgessbar(Polytempo_Event* event)
{
    if (!regionsMap.contains(event->getProperty(eventPropertyString_RegionID)))
        return; // invalid region id

    annotationLayer->requireUpdate();

    Polytempo_GraphicsViewRegion* region = regionsMap[event->getProperty(eventPropertyString_RegionID)];
    int time = event->hasDefinedTime() ? event->getTime() : Polytempo_ScoreScheduler::getInstance()->getScoreTime();
    region->setProgressbar(String(event->getValue().toString()), time, event->getProperty("duration"));
    MessageManager::callAsync([region]() { region->repaint(); });
}
