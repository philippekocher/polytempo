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

#include "Polytempo_GraphicsViewRegion.h"
#include "../../Scheduler/Polytempo_Event.h"
#include "../../Application/PolytempoNetwork/Polytempo_NetworkApplication.h"


Polytempo_GraphicsViewRegion::Polytempo_GraphicsViewRegion(var id)
{
    setOpaque(true);
    borderSize = 0;
    setBufferedToImage(true);

    regionID = id;
    contentType = contentType_Empty;
}

Polytempo_GraphicsViewRegion::~Polytempo_GraphicsViewRegion()
{
    text  = nullptr;
    progressbar = nullptr;
}

void Polytempo_GraphicsViewRegion::paint(Graphics& g)
{    
    if(contentType == contentType_Empty)    return;
    
    g.fillAll (Colours::white);
        
    if(contentType == contentType_Image && image != nullptr)
    {
        imageZoom = imageZoom == INFINITY ? 1 : imageZoom;
        
        float yOffset = (getHeight() - (imageHeight * imageZoom)) * 0.5;
        g.drawImage(*image,
                    0, yOffset, imageWidth * imageZoom, imageHeight * imageZoom,
                    imageLeft, imageTop, imageWidth, imageHeight);
        
    }
    else if(contentType == contentType_Text)
    {
        g.setColour(Colours::black);
        g.setFont(getHeight()); // fit text vertically
        g.drawFittedText(*text,
                         getLocalBounds().reduced(8,0),
                         Justification::left,
                         5,
                         0.00001);
    }
    else if(contentType == contentType_Progressbar)
    {
        g.fillAll(Colours::black.withAlpha(0.1f));
    }
    
    if(borderSize > 0.0) g.drawRect(getLocalBounds());
}

void Polytempo_GraphicsViewRegion::resized()
{
    //DBG("region resized");
    Rectangle <int> parentBounds = getParentComponent()->getBounds();
    
    setBounds(parentBounds.getWidth()   * relativeBounds.getX(),
              parentBounds.getHeight()  * relativeBounds.getY(),
              parentBounds.getWidth()   * relativeBounds.getWidth(),
              parentBounds.getHeight()  * relativeBounds.getHeight());

    
    if(contentType == contentType_Image)
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
                
                float imageWidth, imageHeight;
                if(event->hasProperty(eventPropertyString_Rect))
                {
                    imageWidth = float(event->getProperty(eventPropertyString_Rect).getArray()->getUnchecked(2)) * im->getWidth();
                    imageHeight = float(event->getProperty(eventPropertyString_Rect).getArray()->getUnchecked(3)) * im->getHeight();
                }
                else
                {
                    imageWidth  = im->getWidth();
                    imageHeight = im->getHeight();
                }
                
                if(imageWidth > maxWidth)   maxWidth = imageWidth;
                if(imageHeight > maxHeight) maxHeight = imageHeight;
            }
        }
        
        float widthZoom  = getWidth()/maxWidth;
        float heightZoom = getHeight()/maxHeight;
        
        imageZoom = widthZoom < heightZoom ? widthZoom : heightZoom;
    }
    else if(contentType == contentType_Progressbar)
    {
        progressbar->setBounds(getLocalBounds().reduced(15,10));  // inset rect
    }
}

void Polytempo_GraphicsViewRegion::setRelativeBounds(const Rectangle <float> &newBounds)
{
    relativeBounds = newBounds;
}

void Polytempo_GraphicsViewRegion::clear()
{
    contentType = contentType_Empty;
    
    text  = nullptr;
    progressbar = nullptr;

    setVisible(false);
}

void Polytempo_GraphicsViewRegion::setImage(Image* img, var rect)
{
    contentType = contentType_Image;
    image = img;
    Array < var > r = *rect.getArray();
    
    if(image == nullptr) return;
    if(r.size() != 4)    return;
    
    // to avoid errors:
    if(float(r[2]) < 0) r.set(2,0.0);
    if(float(r[3]) < 0) r.set(3,0.0);

    imageLeft   = image->getWidth()  * float(r[0]); // left
    imageTop    = image->getHeight() * float(r[1]); // top
    imageWidth  = image->getWidth()  * float(r[2]); // width
    imageHeight = image->getHeight() * float(r[3]); // height
    
    //repaint();
    if(getHeight() == 0) resized();
}

void Polytempo_GraphicsViewRegion::setText(String text_)
{
    contentType = contentType_Text;
    text = new String(text_);

    resized();
}

void Polytempo_GraphicsViewRegion::setProgressbar(String text, int time, int duration)
{
    contentType = contentType_Progressbar;
    progressbar = new Polytempo_Progressbar();
    progressbar->setText(text);
    progressbar->setTime(time);
    progressbar->setDuration(duration);
    
    progressbar->setBounds(getLocalBounds().reduced(15,10));  // inset rect

    addAndMakeVisible(progressbar);

    resized();
}

Polytempo_ViewContentType Polytempo_GraphicsViewRegion::getContentType() { return contentType; }
