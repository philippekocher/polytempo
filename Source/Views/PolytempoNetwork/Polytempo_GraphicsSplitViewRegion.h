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

#ifndef __Polytempo_GraphicsSplitViewRegion__
#define __Polytempo_GraphicsSplitViewRegion__

//#include "../../JuceLibraryCode/JuceHeader.h"

#include "Polytempo_Progressbar.h"


class Polytempo_GraphicsSplitViewRegion : public Component
{
public:
    Polytempo_GraphicsSplitViewRegion(var);
    ~Polytempo_GraphicsSplitViewRegion();

    enum ContentType
    {
        contentType_Empty = 0,
        contentType_Image,
        contentType_Text,
        contentType_Progressbar
    };
    
    void paint(Graphics& g);
    void resized();
    
    void setRelativeBounds(const Rectangle <float> &newBounds);
    
    void clear();
    
    void setImage(Image *img, var);
    void setText(String);
    void setProgressbar(String text,
                        int    time,
                        int    duration = 0);

private:
    var regionID;
    Array < ContentType >* types;
    
    int numSplitViews = 2;
    int splitRegionIndex = 0;
    
    float borderSize;
    Rectangle < float > relativeBounds;
    Array < Image* >* images;
    Array < Rectangle < float > >* imageRects;
    HashMap < int, String > texts;
    ScopedPointer < Polytempo_Progressbar > progressbar;
    float sectionAspectRatio, imageAspectRatio;
    float imageZoom;
};
    



#endif   // __Polytempo_GraphicsSplitViewRegion__
