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

void Polytempo_GraphicsViewRegion::paintContent(Graphics& g)
{    
    if(contentType == contentType_Empty)    return;
    
    g.fillAll (Colours::white);
        
    if(contentType == contentType_Image && image != nullptr)
    {
        g.drawImage(*image,
                    targetArea.getX(), targetArea.getY(), targetArea.getWidth(), targetArea.getHeight(),
                    (int)imageLeft, (int)imageTop, (int)imageWidth, (int)imageHeight);
        
    }
    else if(contentType == contentType_Text)
    {
        g.setColour(Colours::black);
        g.setFont((float)getHeight()); // fit text vertically
        g.drawFittedText(*text,
                         getLocalBounds().reduced(8,0),
                         Justification::left,
                         5,
                         0.00001f);
    }
    else if(contentType == contentType_Progressbar)
    {
        g.fillAll(Colours::black.withAlpha(0.1f));
    }
    
    if(borderSize > 0.0) g.drawRect(getLocalBounds());
}

void Polytempo_GraphicsViewRegion::resizeContent()
{
    //DBG("region resized");
    Rectangle <int> parentBounds = getParentComponent()->getBounds();
    
    setBounds((int)(parentBounds.getWidth()   * relativeBounds.getX()),
              (int)(parentBounds.getHeight()  * relativeBounds.getY()),
              (int)(parentBounds.getWidth()   * relativeBounds.getWidth()),
              (int)(parentBounds.getHeight()  * relativeBounds.getHeight()));

    if(contentType == contentType_Image)
    {
        float widthZoom  = getWidth()/imageWidth;
        float heightZoom = getHeight()/imageHeight;
        
        imageZoom = widthZoom < heightZoom ? widthZoom : heightZoom;
        
        if(maxImageZoom > 0.0f && imageZoom > maxImageZoom) imageZoom = maxImageZoom;

		if(imageZoom == INFINITY) imageZoom = 1;

		float yOffset = (getHeight() - (imageHeight * imageZoom)) * 0.5f;
		targetArea = Rectangle<int>(0, yOffset, imageWidth* imageZoom, imageHeight*imageZoom);
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

void Polytempo_GraphicsViewRegion::setViewImage(Image* img, var rect)
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
    
    // calculate zoom to fit image section
    resized();
}

void Polytempo_GraphicsViewRegion::setText(String text_)
{
    contentType = contentType_Text;
    text = new String(text_);

    resized();
}

void Polytempo_GraphicsViewRegion::setProgressbar(String txt, float time, float duration)
{
    contentType = contentType_Progressbar;
    progressbar = new Polytempo_Progressbar();
    progressbar->setText(txt);
    progressbar->setTime(time);
    progressbar->setDuration(duration);
    
    progressbar->setBounds(getLocalBounds().reduced(15,10));  // inset rect

    addAndMakeVisible(progressbar);

    resized();
}

void Polytempo_GraphicsViewRegion::setMaxImageZoom(float maxZoom)
{
    if(maxZoom > 0.0f) maxImageZoom = maxZoom;
    else               maxImageZoom = -1;
}


Polytempo_ViewContentType Polytempo_GraphicsViewRegion::getContentType() { return contentType; }
