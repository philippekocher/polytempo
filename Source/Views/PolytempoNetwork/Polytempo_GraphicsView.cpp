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

#include "Polytempo_GraphicsView.h"
#include "Polytempo_ImageManager.h"
#include "../../Preferences/Polytempo_StoredPreferences.h"
#include "../../Misc/Polytempo_Alerts.h"


Polytempo_GraphicsView::Polytempo_GraphicsView()
{
    setOpaque (true);
	annotationLayer = new Polytempo_GraphicsAnnotationLayer(&regionsMap);
	annotationLayer->setVisible(true);
	addChildComponent(annotationLayer);
}

Polytempo_GraphicsView::~Polytempo_GraphicsView()
{
    deleteAll();
	annotationLayer = nullptr;
}
    
void Polytempo_GraphicsView::paint(Graphics& g)
{
    g.fillAll (Colours::white);
}

void Polytempo_GraphicsView::resized()
{
    HashMap < String, Polytempo_GraphicsViewRegion* >::Iterator it1(regionsMap);
    while(it1.next()) { it1.getValue()->resized(); }

	annotationLayer->setBounds(getLocalBounds());
}

void Polytempo_GraphicsView::eventNotification(Polytempo_Event *event)
{
    if     (event->getType() == eventType_DeleteAll)      deleteAll();
    else if(event->getType() == eventType_ClearAll)       clearAll();
    else if(event->getType() == eventType_AddRegion)      addRegion(event);
    else if(event->getType() == eventType_AddSection)     addSection(event);
    else if(event->getType() == eventType_Image)          displayImage(event);
    else if(event->getType() == eventType_Text)           displayText(event);
    else if(event->getType() == eventType_Progressbar)    displayProgessbar(event);
}

void Polytempo_GraphicsView::deleteAll()
{    
    HashMap < String, Polytempo_GraphicsViewRegion* >::Iterator it1(regionsMap);
	while (it1.next()) { removeChildComponent(it1.getValue()); delete(it1.getValue()); }
	regionsMap.clear();

    sectionBoundsMap.clear();
    sectionImageIDMap.clear();
}

void Polytempo_GraphicsView::clearAll()
{
    HashMap < String, Polytempo_GraphicsViewRegion* >::Iterator it1(regionsMap);
    while(it1.next()) { it1.getValue()->setVisible(false); }
}


void Polytempo_GraphicsView::addRegion(Polytempo_Event *event)
{
    Polytempo_GraphicsViewRegion *region = new Polytempo_GraphicsViewRegion(event->getProperty(eventPropertyString_RegionID));
    addChildComponent(region);
    
    delete regionsMap[event->getProperty(eventPropertyString_RegionID)]; // old region
    regionsMap.set(event->getProperty(eventPropertyString_RegionID),region);
    
    Array<var> r = *event->getProperty(eventPropertyString_Rect).getArray();
    Rectangle<float> bounds = Rectangle<float>(r[0],r[1],r[2],r[3]);
    region->setRelativeBounds(bounds);
    
    region->setMaxImageZoom(event->getProperty(eventPropertyString_MaxZoom));
}

void Polytempo_GraphicsView::addSection(Polytempo_Event *event)
{
    sectionBoundsMap.set(event->getProperty(eventPropertyString_SectionID), event->getProperty(eventPropertyString_Rect));
    sectionImageIDMap.set(event->getProperty(eventPropertyString_SectionID), event->getProperty(eventPropertyString_ImageID));
}

void Polytempo_GraphicsView::displayImage(Polytempo_Event *event)
{
    Polytempo_GraphicsViewRegion      *region = nullptr;
    Image *image = nullptr;
    var rect;
	String imageId;
    
    if(regionsMap.contains(event->getProperty(eventPropertyString_RegionID)))
    {
        region = regionsMap[event->getProperty(eventPropertyString_RegionID)];
    }
    
    if(region == nullptr) // invalid region ID
        return;
    
    if(!event->getProperty(eventPropertyString_SectionID).isVoid())    // a section ID is given
    {
		imageId = sectionImageIDMap[event->getProperty(eventPropertyString_SectionID)];
        image = Polytempo_ImageManager::getInstance()->getImage(imageId);
        rect = sectionBoundsMap[event->getProperty(eventPropertyString_SectionID)];
    }
    else if(!event->getProperty(eventPropertyString_ImageID).isVoid()) // a image ID is given
    {
		imageId = event->getProperty(eventPropertyString_ImageID);
        image = Polytempo_ImageManager::getInstance()->getImage(imageId);
        rect = event->getProperty(eventPropertyString_Rect);
    }

    if(image == nullptr) // invalid image ID
    {
        if(region)      region->setVisible(false);
        return;
    }
    
    // default rectangle [0,0,1,1]
    if(rect == var())
    {
        Array < var > r;
        r.set(0,0); r.set(1,0); r.set(2,1); r.set(3,1);
        rect = r;
    }
    
    region->setImage(image, rect, imageId);
    region->setVisible(true);
    region->repaint();
	annotationLayer->requireUpdate();
}

void Polytempo_GraphicsView::displayText(Polytempo_Event *event)
{
    Polytempo_GraphicsViewRegion      *region = nullptr;
    
    if(regionsMap.contains(event->getProperty(eventPropertyString_RegionID)))
    {
        region = regionsMap[event->getProperty(eventPropertyString_RegionID)];
    }
    else return; // invalid region id
    
    if(event->getProperty("value").isVoid())  // no text given
    {
        if(region)      region->setVisible(false);
        return;
    }
    
    if(region)
    {
        region->setText(String(event->getProperty("value").toString()));
        region->setVisible(true);
        region->repaint();
    }
}

void Polytempo_GraphicsView::displayProgessbar(Polytempo_Event *event)
{
    if(!regionsMap.contains(event->getProperty(eventPropertyString_RegionID)))
        return; // invalid region id
    
    Polytempo_GraphicsViewRegion *region = regionsMap[event->getProperty(eventPropertyString_RegionID)];
    region->setProgressbar(String(event->getValue().toString()), float(event->getTime()), event->getProperty("duration"));
    region->setVisible(true);
    region->repaint();
}
