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

#include "Polytempo_GraphicsSplitViewRegion.h"
#include "../../Scheduler/Polytempo_Event.h"
#include "../../Application/PolytempoNetwork/Polytempo_NetworkApplication.h"


Polytempo_GraphicsSplitViewRegion::Polytempo_GraphicsSplitViewRegion(var id)
{
    setOpaque(true);
    borderSize = 0;
    setBufferedToImage(true);

    regionID = id;
    
    types = new Array < ContentType >();
    for(int i=0;i<numSplitViews;i++) types->set(i,contentType_Empty);
    
    images = new Array < Image* >();
    imageRects = new Array < Rectangle < float > >();
}


Polytempo_GraphicsSplitViewRegion::~Polytempo_GraphicsSplitViewRegion()
{
    //texts  = nullptr;
    progressbar = nullptr;
}

void Polytempo_GraphicsSplitViewRegion::paint(Graphics& g)
{
    g.fillAll (Colours::white);

    for(int i=0;i<numSplitViews;i++)
    {
        if((*types)[i] == contentType_Empty) continue;
        
        float yOffset = float(i) / numSplitViews * getHeight();
        
        if((*types)[i] == contentType_Image)
        {
            Image* img;
            Rectangle < float > imageRect;
            
            if(i < images->size())
            {
                img = images->getUnchecked(i);
                imageRect = imageRects->getUnchecked(i);
            }
            else continue;
            
            //if(img == nullptr) break;
         
            g.drawImage(*img,
                        0, yOffset, imageRect.getWidth() * imageZoom, imageRect.getHeight() * imageZoom,
                        imageRect.getX(), imageRect.getY(), imageRect.getWidth(), imageRect.getHeight());
            
        }
        else if((*types)[i] == contentType_Text)
        {
            String text = texts[i];
            if(text == String::empty) continue;
            
            g.setColour(Colours::black);
            g.setFont(24);
            g.drawFittedText(text,
                             getLocalBounds().withHeight(getHeight()/numSplitViews).withY(yOffset),
                             Justification::horizontallyCentred | Justification::verticallyCentred,
                             5);
        }
        else if((*types)[i] == contentType_Progressbar)
        {
            g.fillAll(Colours::black.withAlpha(0.1f));
        }
    }
    
    if(borderSize > 0.0) g.drawRect(getLocalBounds());
}

void Polytempo_GraphicsSplitViewRegion::resized()
{
    //DBG("region resized");
    Rectangle <int> parentBounds = getParentComponent()->getBounds();
    
    setBounds(parentBounds.getWidth()   * relativeBounds.getX(),
              parentBounds.getHeight()  * relativeBounds.getY(),
              parentBounds.getWidth()   * relativeBounds.getWidth(),
              parentBounds.getHeight()  * relativeBounds.getHeight());

    
    if((*types)[0] == contentType_Image)
    {
        /* 
         find the largest image to be shown in this region
         and calculate the image zoom factor
         */
        
        Polytempo_NetworkApplication* const app = dynamic_cast<Polytempo_NetworkApplication*>(JUCEApplication::getInstance());
        Array < Polytempo_Event* > imageEvents = app->getScore()->getEvents(eventType_Image);
        
        float maxWidth=0, maxHeight=0;
        for(int i=0;i<imageEvents.size();i++)
        {
            Polytempo_Event *event = imageEvents[i];
            if(event->getProperty(eventPropertyString_RegionID) == regionID)
            {
                Image* im = Polytempo_ImageManager::getInstance()->getImage(event->getProperty(eventPropertyString_ImageID));
                if(im == nullptr) continue;
                
                float imageWidth  = float(event->getProperty(eventPropertyString_Rect).getArray()->getUnchecked(2)) * im->getWidth();
                float imageHeight = float(event->getProperty(eventPropertyString_Rect).getArray()->getUnchecked(3)) * im->getHeight();
                
                if(imageWidth > maxWidth)   maxWidth = imageWidth;
                if(imageHeight > maxHeight) maxHeight = imageHeight;
            }
        }
        
        float widthZoom  = getWidth()/maxWidth;
        float heightZoom = getHeight()/numSplitViews/maxHeight;
        
        imageZoom = widthZoom < heightZoom ? widthZoom : heightZoom;
    }
    else if((*types)[0] == contentType_Progressbar)
    {
        progressbar->setBounds(getLocalBounds().reduced(15,10));  // inset rect
    }
}

void Polytempo_GraphicsSplitViewRegion::setRelativeBounds(const Rectangle <float> &newBounds)
{
    relativeBounds = newBounds;
}

void Polytempo_GraphicsSplitViewRegion::clear()
{
    for(int i=0;i<numSplitViews;i++) types->set(i,contentType_Empty);
    
    progressbar = nullptr;

    setVisible(false);
    
    images->clearQuick();
    splitRegionIndex = 0;
    
    texts.clear();
}

void Polytempo_GraphicsSplitViewRegion::setImage(Image* img, var rect)
{
    types->set(splitRegionIndex, contentType_Image);
    images->set(splitRegionIndex, img);
    Array < var > r = *rect.getArray();
    
    if(img == nullptr) return;
    if(r.size() != 4)  return;
    
    // to avoid errors:
    if(float(r[2]) < 0) r.set(2,0.0);
    if(float(r[3]) < 0) r.set(3,0.0);

    Rectangle < float > imageRect(img->getWidth()  * float(r[0]),
                                  img->getHeight() * float(r[1]),
                                  img->getWidth()  * float(r[2]),
                                  img->getHeight() * float(r[3]));
 
    imageRects->set(splitRegionIndex, imageRect);

    
    // update index
    if(++splitRegionIndex == numSplitViews) splitRegionIndex = 0;

    if(getHeight() == 0) resized();
}

void Polytempo_GraphicsSplitViewRegion::setText(String text)
{
    types->set(splitRegionIndex, contentType_Text);
    texts.set(splitRegionIndex, text);
    
    // update index
    if(++splitRegionIndex == numSplitViews) splitRegionIndex = 0;

    if(getHeight() == 0) resized();
}

void Polytempo_GraphicsSplitViewRegion::setProgressbar(String text, int time, int duration)
{
    types->set(splitRegionIndex, contentType_Progressbar);
    progressbar = new Polytempo_Progressbar();
    progressbar->setText(text);
    progressbar->setTime(time);
    progressbar->setDuration(duration);
    
    progressbar->setBounds(getLocalBounds().reduced(15,10));  // inset rect

    addAndMakeVisible(progressbar);

    resized();
}
