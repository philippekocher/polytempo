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
#define contentLayoutString_Row    "row"
#define contentLayoutString_Column "column"

enum Polytempo_ViewContentXAlignment
{
    contentXAlignment_Left = 0,
    contentXAlignment_Center,
    contentXAlignment_Right,
};
#define contentXAlignmentString_Left   "left"
#define contentXAlignmentString_Center "center"
#define contentXAlignmentString_Right  "right"

enum Polytempo_ViewContentYAlignment
{
    contentYAlignment_Top = 0,
    contentYAlignment_Center,
    contentYAlignment_Bottom
};
#define contentYAlignmentString_Top    "top"
#define contentYAlignmentString_Center "center"
#define contentYAlignmentString_Bottom "bottom"


class Polytempo_GraphicsViewRegion : public Component, public ChangeListener
{
public:
    Polytempo_GraphicsViewRegion(var = var());
    ~Polytempo_GraphicsViewRegion() override;

    void paint(Graphics& g) override;
    void resized() override;
    void setImage(Image* img, var rect, String imageId, bool append = false);

    void setRelativeBounds(const Rectangle<float>& newBounds);

    void clear(bool keepImages = false);

    void setText(String text);
    void setProgressbar(String text,
                        int time,
                        float duration = 0);

    void setMaxImageZoom(float maxZoom);
    void setLayout(String layout);
    void setAlignment(String xAlignment, String yAlignment);

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
    Polytempo_ViewContentXAlignment contentXAlignment;
    Polytempo_ViewContentYAlignment contentYAlignment;

    Rectangle<float> relativeBounds;

    std::vector<displayedImage> displayedImages;

    std::unique_ptr<String> text;
    std::unique_ptr<Polytempo_Progressbar> progressbar;
    float imageZoom = 1;
    float maxImageZoom = -1;

    bool allowAnnotations;
};
