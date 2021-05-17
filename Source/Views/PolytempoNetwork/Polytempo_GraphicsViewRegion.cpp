#include "Polytempo_GraphicsViewRegion.h"
#include "Polytempo_GraphicsAnnotationManager.h"

Polytempo_GraphicsViewRegion::Polytempo_GraphicsViewRegion(var id)
{
    setOpaque(true);
    borderSize = 0;
    setBufferedToImage(true);

    regionID = id;
    contentType = contentType_Empty;
    contentLayout = contentLayout_Row;
    allowAnnotations = false;

    Polytempo_GraphicsAnnotationManager::getInstance()->addChangeListener(this);
}

Polytempo_GraphicsViewRegion::~Polytempo_GraphicsViewRegion()
{
    text = nullptr;
    progressbar = nullptr;

    Polytempo_GraphicsAnnotationManager::getInstance()->removeChangeListener(this);
}

void Polytempo_GraphicsViewRegion::paint(Graphics& g)
{
    if (contentType == contentType_Empty) return;

    g.fillAll(Colours::white);

    if (contentType == contentType_Image && imageRegions.size() > 0)
    {
        Polytempo_GraphicsAnnotationManager::eAnnotationMode annotationMode = Polytempo_GraphicsAnnotationManager::getInstance()->getAnnotationMode();
        if (annotationMode == Polytempo_GraphicsAnnotationManager::Standard || annotationMode == Polytempo_GraphicsAnnotationManager::Edit)
        {
            g.fillAll(Colours::lightgrey.brighter(0.7f));
        }

        for (imageRegion region : imageRegions)
        {
            g.drawImage(*region.image,
                        region.targetArea.getX(), region.targetArea.getY(), region.targetArea.getWidth(), region.targetArea.getHeight(),
                        region.imageRect.getX(), region.imageRect.getY(), region.imageRect.getWidth(), region.imageRect.getHeight());
        }
    }
    else if (contentType == contentType_Text)
    {
        g.setColour(Colours::black);
        g.setFont((float)getHeight()); // fit text vertically
        g.drawFittedText(*text,
                         getLocalBounds().reduced(8, 0),
                         Justification::left,
                         5,
                         0.00001f);
    }
    else if (contentType == contentType_Progressbar)
    {
        g.fillAll(Colours::black.withAlpha(0.1f));
    }

    if (borderSize > 0.0) g.drawRect(getLocalBounds());
}

void Polytempo_GraphicsViewRegion::resized()
{
    Rectangle<int> parentBounds = getParentComponent()->getBounds();

    setBounds((int)(parentBounds.getWidth() * relativeBounds.getX()),
              (int)(parentBounds.getHeight() * relativeBounds.getY()),
              (int)(parentBounds.getWidth() * relativeBounds.getWidth()),
              (int)(parentBounds.getHeight() * relativeBounds.getHeight()));

    if (contentType == contentType_Image)
    {
        float totalWidth = 0, maxWidth = 0;
        float totalHeight = 0, maxHeight = 0;
        for (imageRegion region : imageRegions)
        {
            float regionWidth = region.imageRect.getWidth();
            float regionHeight = region.imageRect.getHeight();
            totalWidth += regionWidth;
            if (regionWidth > maxWidth) maxWidth = regionWidth;
            totalHeight += regionHeight;
            if (regionHeight > maxHeight) maxHeight = regionHeight;
        }
        
        float widthZoom, heightZoom;
        if (contentLayout == contentLayout_Row)
        {
            widthZoom = getWidth() / totalWidth;
            heightZoom = getHeight() / maxHeight;
        }
        else
        {
            widthZoom = getWidth() / maxWidth;
            heightZoom = getHeight() / totalHeight;
        }
    
        imageZoom = widthZoom < heightZoom ? widthZoom : heightZoom;

        if (maxImageZoom > 0.0f && imageZoom > maxImageZoom) imageZoom = maxImageZoom;

        if (imageZoom == INFINITY) imageZoom = 1;

        float x = 0, y;
        if (contentLayout == contentLayout_Row)
            y = (getHeight() - (maxHeight * imageZoom)) * 0.5f;
        else
            y = (getHeight() - (totalHeight * imageZoom)) * 0.5f;

        for (imageRegion &region : imageRegions)
        {
            region.targetArea = Rectangle<int>(int(x), int(y), int(region.imageRect.getWidth() * imageZoom), int(region.imageRect.getHeight() * imageZoom));
            
            if(contentLayout == contentLayout_Row)
                x += region.imageRect.getWidth() * imageZoom;
            else
                y += region.imageRect.getHeight() * imageZoom;
        }
    }
    else if (contentType == contentType_Progressbar)
    {
        progressbar->setBounds(getLocalBounds().reduced(15, 10)); // inset rect
    }

    if (imageRegions.size() == 0)
        allowAnnotations = false;
    else
    {
        screenToImage = AffineTransform::translation(-float(getX() + imageRegions[0].targetArea.getX()), -float(getY() + imageRegions[0].targetArea.getY()));
        screenToImage = screenToImage.followedBy(AffineTransform::scale(imageRegions[0].imageRect.getWidth() / float(imageRegions[0].targetArea.getWidth()), imageRegions[0].imageRect.getHeight() / float(imageRegions[0].targetArea.getHeight())));
        screenToImage = screenToImage.followedBy(AffineTransform::translation(imageRegions[0].imageRect.getX(), imageRegions[0].imageRect.getY()));

        imageToScreen = screenToImage.inverted();
        allowAnnotations = true;
    }
}

void Polytempo_GraphicsViewRegion::setRelativeBounds(const Rectangle<float>& newBounds)
{
    relativeBounds = newBounds;
}

void Polytempo_GraphicsViewRegion::clear()
{
    contentType = contentType_Empty;

    text = nullptr;
    progressbar = nullptr;

    setVisible(false);
}

void Polytempo_GraphicsViewRegion::setImage(Image* img, var rect, String imageId, bool append)
{
    contentType = contentType_Image;
    if (!append) imageRegions.clear();

    Array<var> r = *rect.getArray();

    if (img == nullptr) return;
    if (r.size() != 4) return;

    // to avoid errors:
    if (float(r[2]) < 0) r.set(2, 0.0);
    if (float(r[3]) < 0) r.set(3, 0.0);

    Rectangle<int> imageRect;
    imageRect.setX(int(img->getWidth() * float(r[0]))); // left
    imageRect.setY(int(img->getHeight() * float(r[1]))); // top
    imageRect.setWidth(int(img->getWidth() * float(r[2]))); // width
    imageRect.setHeight(int(img->getHeight() * float(r[3]))); // height
        
    imageRegions.push_back({img, imageId, imageRect});

    // calculate zoom to fit image section
    resized();
}

void Polytempo_GraphicsViewRegion::setText(String text_)
{
    contentType = contentType_Text;
    text.reset(new String(text_));

    resized();
}

void Polytempo_GraphicsViewRegion::setProgressbar(String txt, float time, float duration)
{
    contentType = contentType_Progressbar;
    progressbar.reset(new Polytempo_Progressbar());
    progressbar->setText(txt);
    progressbar->setTime(int(time));
    progressbar->setDuration(duration);

    progressbar->setBounds(getLocalBounds().reduced(15, 10)); // inset rect

    addAndMakeVisible(progressbar.get());

    resized();
}

void Polytempo_GraphicsViewRegion::setMaxImageZoom(float maxZoom)
{
    if (maxZoom > 0.0f) maxImageZoom = maxZoom;
    else maxImageZoom = -1;
}

void Polytempo_GraphicsViewRegion::setLayout(String layout)
{
    if(layout == "column")   contentLayout = contentLayout_Column;
    else if(layout == "row") contentLayout = contentLayout_Row;
}

Polytempo_ViewContentType Polytempo_GraphicsViewRegion::getContentType() { return contentType; }

AffineTransform& Polytempo_GraphicsViewRegion::getImageToScreenTransform()
{
    return imageToScreen;
}

AffineTransform& Polytempo_GraphicsViewRegion::getScreenToImageTransform()
{
    return screenToImage;
}

bool Polytempo_GraphicsViewRegion::annotationsAllowed() const
{
    return allowAnnotations;
}

String Polytempo_GraphicsViewRegion::getImageID() const
{
    return "-1"; //currentImageId;
}

Point<float> Polytempo_GraphicsViewRegion::getImageCoordinatesAt(Point<int> screenPoint) const
{
    float x = float(screenPoint.getX());
    float y = float(screenPoint.getY());
    screenToImage.transformPoint<float>(x, y);
    return Point<float>(x, y);
}

bool Polytempo_GraphicsViewRegion::imageRectangleContains(Point<float> point) const
{
    return false; //currentImageRectangle.contains(point);
}

void Polytempo_GraphicsViewRegion::changeListenerCallback(ChangeBroadcaster*)
{
    repaint();
}
