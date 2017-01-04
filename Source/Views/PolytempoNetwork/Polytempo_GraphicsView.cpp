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
#include "Polytempo_GraphicsSplitViewRegion.h"
#include "../../Preferences/Polytempo_StoredPreferences.h"
#include "../../Misc/Polytempo_Alerts.h"


Polytempo_GraphicsView::Polytempo_GraphicsView()
{
    setOpaque (true);
}

Polytempo_GraphicsView::~Polytempo_GraphicsView()
{
    deleteAll();
}
    
void Polytempo_GraphicsView::paint(Graphics& g)
{
    g.fillAll (Colours::white);
}

void Polytempo_GraphicsView::resized()
{
    HashMap < String, Polytempo_GraphicsViewRegion* >::Iterator it1(regionsMap);
    while(it1.next()) { it1.getValue()->resized(); }

    HashMap < String, Polytempo_GraphicsSplitViewRegion* >::Iterator it2(splitRegionsMap);
    while(it2.next()) { it2.getValue()->resized(); }
}

void Polytempo_GraphicsView::eventNotification(Polytempo_Event *event)
{
    if     (event->getType() == eventType_DeleteAll)      deleteAll();
    else if(event->getType() == eventType_ClearAll)       clearAll();
    else if(event->getType() == eventType_AddRegion)      addRegion(event);
    else if(event->getType() == eventType_AddSplitRegion) addSplitRegion(event);
    else if(event->getType() == eventType_Image)          displayImage(event);
    else if(event->getType() == eventType_Text)           displayText(event);
    else if(event->getType() == eventType_Progressbar)    displayProgessbar(event);
}

void Polytempo_GraphicsView::deleteAll()
{    
    regionsMap.clear();
    splitRegionsMap.clear();
    deleteAllChildren();
    
    regionBoundsMap.clear();
}

void Polytempo_GraphicsView::clearAll()
{
    HashMap < String, Polytempo_GraphicsViewRegion* >::Iterator it1(regionsMap);
    while(it1.next()) { it1.getValue()->setVisible(false); }

    HashMap < String, Polytempo_GraphicsSplitViewRegion* >::Iterator it2(splitRegionsMap);
    while(it2.next()) { it2.getValue()->clear(); }
}


void Polytempo_GraphicsView::addRegion(Polytempo_Event *event)
{
    Polytempo_GraphicsViewRegion *region = new Polytempo_GraphicsViewRegion(event->getProperty(eventPropertyString_RegionID));
    addChildComponent(region);
    
    Array<var> r = *event->getProperty(eventPropertyString_Rect).getArray();
    Rectangle<float> bounds = Rectangle<float>::Rectangle(r[0],r[1],r[2],r[3]);
    
    regionBoundsMap.set(event->getProperty(eventPropertyString_RegionID), bounds);

    delete regionsMap[event->getProperty(eventPropertyString_RegionID)]; // old region
    regionsMap.set(event->getProperty(eventPropertyString_RegionID),region);
    
    region->setRelativeBounds(bounds);
}

void Polytempo_GraphicsView::addSplitRegion(Polytempo_Event *event)
{
    Polytempo_GraphicsSplitViewRegion *splitRegion = new Polytempo_GraphicsSplitViewRegion(event->getProperty(eventPropertyString_RegionID));
    addChildComponent(splitRegion);
    
    Array<var> r = *event->getProperty(eventPropertyString_Rect).getArray();
    Rectangle<float> bounds = Rectangle<float>::Rectangle(r[0],r[1],r[2],r[3]);
    
    regionBoundsMap.set(event->getProperty(eventPropertyString_RegionID), bounds);
    splitRegionsMap.set(event->getProperty(eventPropertyString_RegionID), splitRegion);
    
    splitRegion->setRelativeBounds(bounds);
}

void Polytempo_GraphicsView::displayImage(Polytempo_Event *event)
{
    Polytempo_GraphicsViewRegion      *region = nullptr;
    Polytempo_GraphicsSplitViewRegion *splitRegion = nullptr;
    
    if(regionsMap.contains(event->getProperty(eventPropertyString_RegionID)))
    {
        region = regionsMap[event->getProperty(eventPropertyString_RegionID)];
    }
    else if(splitRegionsMap.contains(event->getProperty(eventPropertyString_RegionID)))
    {
        splitRegion = splitRegionsMap[event->getProperty(eventPropertyString_RegionID)];
    }
    else return; // invalid region id
    
    if(event->getProperty(eventPropertyString_ImageID).isVoid())  // no imageID given
    {
        if(region)      region->setVisible(false);
        if(splitRegion) splitRegion->clear();
        return;
    }
    
    Image *image = Polytempo_ImageManager::getInstance()->getImage(event->getProperty(eventPropertyString_ImageID));

    if(image == nullptr) // invalid image ID
    {
        if(region)      region->setVisible(false);
        if(splitRegion) splitRegion->clear();
        return;
    }
    
    var rect = event->getProperty(eventPropertyString_Rect);
    if(rect == var::null)
    {
        Array < var > r;
        r.set(0,0); r.set(1,0); r.set(2,1); r.set(3,1);
        rect = r;
    }
    
    if(region)
    {
        region->setImage(image, rect);
        region->setVisible(true);
        region->repaint();
    }
    else if(splitRegion)
    {
        splitRegion->setImage(image, rect);
        splitRegion->setVisible(true);
        splitRegion->repaint();
    }
}

void Polytempo_GraphicsView::displayText(Polytempo_Event *event)
{
    Polytempo_GraphicsViewRegion      *region = nullptr;
    Polytempo_GraphicsSplitViewRegion *splitRegion = nullptr;
    
    if(regionsMap.contains(event->getProperty(eventPropertyString_RegionID)))
    {
        region = regionsMap[event->getProperty(eventPropertyString_RegionID)];
    }
    else if(splitRegionsMap.contains(event->getProperty(eventPropertyString_RegionID)))
    {
        splitRegion = splitRegionsMap[event->getProperty(eventPropertyString_RegionID)];
    }
    else return; // invalid region id
    
    if(event->getProperty("value").isVoid())  // no text given
    {
        if(region)      region->setVisible(false);
        if(splitRegion) splitRegion->clear();
        return;
    }
    
    if(region)
    {
        region->setText(String(event->getProperty("value").toString()));
        region->setVisible(true);
        region->repaint();
    }
    else if(splitRegion)
    {
        splitRegion->setText(String(event->getProperty("value").toString()));
        splitRegion->setVisible(true);
        splitRegion->repaint();
    }
}

void Polytempo_GraphicsView::displayProgessbar(Polytempo_Event *event)
{
    if(!regionsMap.contains(event->getProperty(eventPropertyString_RegionID)))
        return; // invalid region id
    
    Polytempo_GraphicsViewRegion *region = regionsMap[event->getProperty(eventPropertyString_RegionID)];
    region->setProgressbar(String(event->getProperty("value").toString()),event->getTime(), event->getProperty("duration"));
    region->setVisible(true);
    region->repaint();
}
