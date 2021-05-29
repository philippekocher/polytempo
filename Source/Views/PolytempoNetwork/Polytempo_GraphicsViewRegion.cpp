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

    if (contentType == contentType_Image && displayedImages.size() > 0)
    {
        Polytempo_GraphicsAnnotationManager::eAnnotationMode annotationMode = Polytempo_GraphicsAnnotationManager::getInstance()->getAnnotationMode();
        if (annotationMode == Polytempo_GraphicsAnnotationManager::Standard || annotationMode == Polytempo_GraphicsAnnotationManager::Edit)
        {
            g.fillAll(Colours::lightgrey.brighter(0.7f));
        }

        for (displayedImage img : displayedImages)
        {
            g.drawImage(*img.image,
                        img.targetArea.getX(), img.targetArea.getY(), img.targetArea.getWidth(), img.targetArea.getHeight(),
                        img.imageRect.getX(), img.imageRect.getY(), img.imageRect.getWidth(), img.imageRect.getHeight());
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
        for (displayedImage img : displayedImages)
        {
            float imageWidth = img.imageRect.getWidth();
            float imageHeight = img.imageRect.getHeight();
            totalWidth += imageWidth;
            if (imageWidth > maxWidth) maxWidth = imageWidth;
            totalHeight += imageHeight;
            if (imageHeight > maxHeight) maxHeight = imageHeight;
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

        for (displayedImage &img : displayedImages)
        {
            img.targetArea = Rectangle<int>(int(x), int(y), int(img.imageRect.getWidth() * imageZoom), int(img.imageRect.getHeight() * imageZoom));
            
            img.screenToImage = AffineTransform::translation(-float(getX() + img.targetArea.getX()), -float(getY() + img.targetArea.getY()));
            img.screenToImage = img.screenToImage.followedBy(AffineTransform::scale(img.imageRect.getWidth() / float(img.targetArea.getWidth()), img.imageRect.getHeight() / float(displayedImages[0].targetArea.getHeight())));
            img.screenToImage = img.screenToImage.followedBy(AffineTransform::translation(img.imageRect.getX(), img.imageRect.getY()));

            img.imageToScreen = img.screenToImage.inverted();

            if(contentLayout == contentLayout_Row)
                x += img.imageRect.getWidth() * imageZoom;
            else
                y += img.imageRect.getHeight() * imageZoom;
        }
    }
    else if (contentType == contentType_Progressbar)
    {
        progressbar->setBounds(getLocalBounds().reduced(15, 10)); // inset rect
    }

    if (displayedImages.size() == 0)
        allowAnnotations = false;
    else
        allowAnnotations = true;
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
    if (!append) displayedImages.clear();

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
        
    displayedImages.push_back({img, imageId, imageRect});

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

bool Polytempo_GraphicsViewRegion::annotationsAllowed() const
{
    return allowAnnotations;
}

std::vector<Polytempo_GraphicsViewRegion::displayedImage> Polytempo_GraphicsViewRegion::getDisplayedImages()
{
    return displayedImages;
}

Polytempo_GraphicsViewRegion::displayedImage* Polytempo_GraphicsViewRegion::getDisplayedImageAt(Point<int> point)
{
    for (displayedImage &img : displayedImages)
    {
        if(img.targetArea.contains(point))
            return &img;
    }
    return nullptr;
}

void Polytempo_GraphicsViewRegion::changeListenerCallback(ChangeBroadcaster*)
{
    repaint();
}
