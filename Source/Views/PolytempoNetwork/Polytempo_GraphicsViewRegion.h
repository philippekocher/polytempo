#pragma once

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