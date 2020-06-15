#include "Polytempo_GraphicsViewRegion.h"
#include "Polytempo_GraphicsAnnotationManager.h"

Polytempo_GraphicsViewRegion::Polytempo_GraphicsViewRegion(var id)
{
    setOpaque(true);
    borderSize = 0;
    setBufferedToImage(true);

    regionID = id;
    contentType = contentType_Empty;
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

    if (contentType == contentType_Image && image != nullptr)
    {
        Polytempo_GraphicsAnnotationManager::eAnnotationMode annotationMode = Polytempo_GraphicsAnnotationManager::getInstance()->getAnnotationMode();
        if (annotationMode == Polytempo_GraphicsAnnotationManager::Standard || annotationMode == Polytempo_GraphicsAnnotationManager::Edit)
        {
            g.fillAll(Colours::lightgrey.brighter(0.7f));
        }

        g.drawImage(*image,
                    targetArea.getX(), targetArea.getY(), targetArea.getWidth(), targetArea.getHeight(),
                    (int)imageLeft, (int)imageTop, (int)imageWidth, (int)imageHeight);
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
        float widthZoom = getWidth() / imageWidth;
        float heightZoom = getHeight() / imageHeight;

        imageZoom = widthZoom < heightZoom ? widthZoom : heightZoom;

        if (maxImageZoom > 0.0f && imageZoom > maxImageZoom) imageZoom = maxImageZoom;

        if (imageZoom == INFINITY) imageZoom = 1;

        int yOffset = int((getHeight() - (imageHeight * imageZoom)) * 0.5f);
        targetArea = Rectangle<int>(0, yOffset, int(imageWidth * imageZoom), int(imageHeight * imageZoom));
    }
    else if (contentType == contentType_Progressbar)
    {
        progressbar->setBounds(getLocalBounds().reduced(15, 10)); // inset rect
    }

    if (currentImageRectangle.isEmpty())
        allowAnnotations = false;
    else
    {
        screenToImage = AffineTransform::translation(-float(getX() + targetArea.getX()), -float(getY() + targetArea.getY()));
        screenToImage = screenToImage.followedBy(AffineTransform::scale(currentImageRectangle.getWidth() / float(targetArea.getWidth()), currentImageRectangle.getHeight() / float(targetArea.getHeight())));
        screenToImage = screenToImage.followedBy(AffineTransform::translation(currentImageRectangle.getX(), currentImageRectangle.getY()));

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

void Polytempo_GraphicsViewRegion::setImage(Image* img, var rect, String imageId)
{
    Array<var> r = *rect.getArray();
    float relTopLeftX = float(r[0]);
    float relTopLeftY = float(r[1]);
    float relWidth = float(r[2]);
    float relHeight = float(r[3]);
    float imgWidth = float(img->getWidth());
    float imgHeight = float(img->getHeight());
    currentImageRectangle = Rectangle<float>(
        imgWidth * relTopLeftX,
        imgHeight * relTopLeftY,
        imgWidth * relWidth,
        imgHeight * relHeight);

    currentImageId = imageId;

    setViewImage(img, rect);

    resized();
}

void Polytempo_GraphicsViewRegion::setViewImage(Image* img, var rect)
{
    contentType = contentType_Image;
    image = img;
    Array<var> r = *rect.getArray();

    if (image == nullptr) return;
    if (r.size() != 4) return;

    // to avoid errors:
    if (float(r[2]) < 0) r.set(2, 0.0);
    if (float(r[3]) < 0) r.set(3, 0.0);

    imageLeft = image->getWidth() * float(r[0]); // left
    imageTop = image->getHeight() * float(r[1]); // top
    imageWidth = image->getWidth() * float(r[2]); // width
    imageHeight = image->getHeight() * float(r[3]); // height

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
    return currentImageId;
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
    return currentImageRectangle.contains(point);
}

void Polytempo_GraphicsViewRegion::changeListenerCallback(ChangeBroadcaster*)
{
    repaint();
}
