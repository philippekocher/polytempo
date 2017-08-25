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

#ifndef __Polytempo_GraphicsViewRegion__
#define __Polytempo_GraphicsViewRegion__


#include "Polytempo_Progressbar.h"
#include "Polytempo_GraphicsEditableRegion.h"


enum Polytempo_ViewContentType
{
    contentType_Empty = 0,
    contentType_Image,
    contentType_Text,
    contentType_Progressbar
};

class Polytempo_GraphicsViewRegion : public Polytempo_GraphicsEditableRegion
{
public:
    Polytempo_GraphicsViewRegion(var = var::null);
    ~Polytempo_GraphicsViewRegion();

    void paintContent(Graphics& g) override;
    void resizeContent() override;
    
    void setRelativeBounds(const Rectangle <float> &newBounds);
    
    void clear();
    
    void setText(String text);
    void setProgressbar(String text,
                        float    time,
                        int    duration = 0);
    
    void setMaxImageZoom(float maxZoom);
    
    Polytempo_ViewContentType getContentType();
    
private:
	void setViewImage(Image* img, var rect) override;

    var regionID;
    Polytempo_ViewContentType contentType;
    
    float borderSize;
    Rectangle < float > relativeBounds;
    Image  *image;
    ScopedPointer < String > text;
    ScopedPointer < Polytempo_Progressbar > progressbar;
    float imageLeft, imageTop, imageWidth, imageHeight;
    float imageZoom = 1;
    float maxImageZoom = -1;
};
    



#endif   // __Polytempo_GraphicsViewRegion__
