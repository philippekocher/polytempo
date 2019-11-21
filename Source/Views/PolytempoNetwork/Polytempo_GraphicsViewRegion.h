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


enum Polytempo_ViewContentType
{
    contentType_Empty = 0,
    contentType_Image,
    contentType_Text,
    contentType_Progressbar
};

class Polytempo_GraphicsViewRegion : public Component, public ChangeListener
{
public:
    Polytempo_GraphicsViewRegion(var = var());
    ~Polytempo_GraphicsViewRegion();
    
    void paint(Graphics& g) override;
    void resized() override;
	void setImage(Image *img, var rect, String imageId);

    void setRelativeBounds(const Rectangle <float> &newBounds);
    
    void clear();
    
    void setText(String text);
    void setProgressbar(String text,
                        float    time,
                        float    duration = 0);
    
    void setMaxImageZoom(float maxZoom);
    
    Polytempo_ViewContentType getContentType();
	AffineTransform& getImageToScreenTransform();
	AffineTransform& getScreenToImageTransform(); 
	bool annotationsAllowed() const;
	String getImageID() const;
	Point<float> getImageCoordinatesAt(Point<int> screenPoint) const;
	bool imageRectangleContains(Point<float> point) const;


private:
	void setViewImage(Image* img, var rect);
    void changeListenerCallback(ChangeBroadcaster *source) override;

    var regionID;
    Polytempo_ViewContentType contentType;
    
    float borderSize;
    Rectangle < float > relativeBounds;
    Image  *image;
	std::unique_ptr < String > text;
	std::unique_ptr < Polytempo_Progressbar > progressbar;
	float imageZoom = 1;
	float maxImageZoom = -1;    

	Rectangle<int> targetArea;
	bool allowAnnotations;

	Rectangle<float> currentImageRectangle;
	String currentImageId;

	AffineTransform imageToScreen;
	AffineTransform screenToImage;
	float imageLeft, imageTop, imageWidth, imageHeight;
};
    



#endif   // __Polytempo_GraphicsViewRegion__
