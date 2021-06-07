#pragma once

#include "Polytempo_Progressbar.h"

enum Polytempo_ViewContentType
{
    contentType_Empty = 0,
    contentType_Image,
    contentType_Text,
    contentType_Progressbar
};

enum Polytempo_ViewContentLayout
{
    contentLayout_Row = 0,
    contentLayout_Column
};

class Polytempo_GraphicsViewRegion : public Component, public ChangeListener
{
public:
    Polytempo_GraphicsViewRegion(var = var());
    ~Polytempo_GraphicsViewRegion();

    void paint(Graphics& g) override;
    void resized() override;
    void setImage(Image* img, var rect, String imageId, bool append = false);

    void setRelativeBounds(const Rectangle<float>& newBounds);

    void clear();

    void setText(String text);
    void setProgressbar(String text,
                        float time,
                        float duration = 0);

    void setMaxImageZoom(float maxZoom);
    void setLayout(String layout);

    Polytempo_ViewContentType getContentType();
    bool annotationsAllowed() const;

    struct displayedImage
    {
        Image* image;
        const String imageId;
        const Rectangle<int> imageRect;
        Rectangle<int> targetArea;
        AffineTransform imageToScreen;
        AffineTransform screenToImage;
    };

    std::vector<displayedImage> getDisplayedImages();
    displayedImage* getDisplayedImageAt(Point<int>);

private:
    void changeListenerCallback(ChangeBroadcaster* source) override;

    var regionID;
    Polytempo_ViewContentType contentType;
    Polytempo_ViewContentLayout contentLayout;

    float borderSize;
    Rectangle<float> relativeBounds;

    std::vector<displayedImage> displayedImages;

    std::unique_ptr<String> text;
    std::unique_ptr<Polytempo_Progressbar> progressbar;
    float imageZoom = 1;
    float maxImageZoom = -1;

    bool allowAnnotations;
};
