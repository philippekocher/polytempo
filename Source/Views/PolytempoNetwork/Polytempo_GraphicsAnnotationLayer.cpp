#include "JuceHeader.h"
#include "Polytempo_GraphicsAnnotationLayer.h"
#include "Polytempo_GraphicsAnnotationManager.h"
#include "Polytempo_GraphicsPalette.h"
#include <float.h>

Polytempo_GraphicsAnnotationLayer::Polytempo_GraphicsAnnotationLayer(HashMap<String, Polytempo_GraphicsViewRegion*>* pRegionMap)
{
    this->m_pRegionMap = pRegionMap;
    status = Default;
    setAlwaysOnTop(true);
    startTimer(TIMER_ID_REPAINT, MIN_INTERVAL_BETWEEN_REPAINTS_MS);
    Polytempo_GraphicsPalette::getInstance()->setAnnotationLayer(this);
    setWantsKeyboardFocus(true);

    Polytempo_GraphicsAnnotationManager::getInstance()->addChangeListener(this);
}

Polytempo_GraphicsAnnotationLayer::~Polytempo_GraphicsAnnotationLayer()
{
    stopTimer(TIMER_ID_REPAINT);
    stopTimer(TIMER_ID_AUTO_ACCEPT);
    Polytempo_GraphicsAnnotationManager::getInstance()->removeChangeListener(this);
    Polytempo_GraphicsAnnotationManager::getInstance()->resetAnnotationPending(this);
}

void Polytempo_GraphicsAnnotationLayer::paint(Graphics& g)
{
    bool doFullRepaint = fullUpdateRequired.compareAndSetBool(false, true);
    if (doFullRepaint)
    {
        prepareAnnotationLayer();
    }
    if (annotationImage != nullptr)
    {
        g.drawImage(*annotationImage,
                    0, 0, getWidth(), getHeight(),
                    int(0), int(0), int(annotationImage->getWidth()), int(annotationImage->getHeight()));
    }

    if ((status == FreehandEditing || status == Selected) && temporaryAnnotation.m_displayedImage != nullptr)
    {
        g.addTransform(temporaryAnnotation.m_displayedImage->imageToScreen);
        paintAnnotation(g, &temporaryAnnotation, true, Colours::red);
    }
    else if (status == DragDrop && temporaryAnnotationMove.m_displayedImage != nullptr)
    {
        g.addTransform(temporaryAnnotation.m_displayedImage->imageToScreen);
        paintAnnotation(g, &temporaryAnnotationMove, true, Colours::red);
    }
}

void Polytempo_GraphicsAnnotationLayer::resized()
{
    fullUpdateRequired.set(true);
    repaint();
}

void Polytempo_GraphicsAnnotationLayer::prepareAnnotationLayer()
{
    annotationImage.reset(new Image(Image::ARGB, getWidth(), getHeight(), true));

    bool anchorFlag = Polytempo_GraphicsAnnotationManager::getInstance()->getAnchorFlag();

    HashMap<String, Polytempo_GraphicsViewRegion*>::Iterator it1(*m_pRegionMap);
    while (it1.next())
    {
        OwnedArray<Polytempo_GraphicsAnnotation> annotations;
        for(Polytempo_GraphicsViewRegion::displayedImage img : it1.getValue()->getDisplayedImages())
        {
            Polytempo_GraphicsAnnotationManager::getInstance()->getAnnotationsForImage(img.imageId, &annotations);
            for (Polytempo_GraphicsAnnotation* annotation : annotations)
            {
                if (img.imageId == annotation->m_imageId &&
                    img.imageRect.contains(int(annotation->m_referencePoint.x), int(annotation->m_referencePoint.y)) &&
                    (status != FreehandEditing || temporaryAnnotation.m_id != annotation->m_id))
                {
                    Graphics g(*annotationImage);
                    g.addTransform(img.imageToScreen);
                    paintAnnotation(g, annotation, anchorFlag, Colours::blue);
                }
            }
        }
    }
}

void Polytempo_GraphicsAnnotationLayer::paintAnnotation(Graphics& g, const Polytempo_GraphicsAnnotation* annotation, bool anchorFlag, Colour anchorColor)
{
    PathStrokeType strokeType(PathStrokeType::rounded);
    strokeType.setStrokeThickness(annotation->m_lineWeight);

    g.setColour(annotation->m_color);
    if (!annotation->m_freeHandPath.isEmpty())
        g.strokePath(annotation->m_freeHandPath, strokeType);

    if (!annotation->m_text.isEmpty())
    {
        g.setFont(annotation->m_fontSize);
        g.drawSingleLineText(annotation->m_text, int(annotation->m_referencePoint.getX()) + 10, int(annotation->m_referencePoint.getY()));
    }

    if (anchorFlag)
    {
        g.setColour(anchorColor);
        g.fillRoundedRectangle(annotation->m_referencePoint.getX() - 8.0f, annotation->m_referencePoint.getY() - 8.0f, 16.0f, 16.0f, 4.0f);
    }
}

void Polytempo_GraphicsAnnotationLayer::requireUpdate()
{
    fullUpdateRequired.set(true);
}

Polytempo_GraphicsViewRegion::displayedImage* Polytempo_GraphicsAnnotationLayer::getDisplayedImageAt(Point<int> mouse) const
{
    HashMap<String, Polytempo_GraphicsViewRegion*>::Iterator it1(*m_pRegionMap);
    while (it1.next())
    {
        Polytempo_GraphicsViewRegion* pRegion = it1.getValue();
        if (pRegion->annotationsAllowed() && pRegion->getBounds().contains(mouse))
        {
            return pRegion->getDisplayedImageAt(mouse);
        }
    }
    return nullptr;
}

Point<float> Polytempo_GraphicsAnnotationLayer::getImageCoordinates(Polytempo_GraphicsViewRegion::displayedImage* img, Point<int> point) const
{
    float x = float(point.getX());
    float y = float(point.getY());
    img->screenToImage.transformPoint<float>(x, y);
    return Point<float>(x, y);
}

void Polytempo_GraphicsAnnotationLayer::handleStartEditing(Point<int> mousePosition)
{
    Polytempo_GraphicsAnnotationManager::eAnnotationMode mode = Polytempo_GraphicsAnnotationManager::getInstance()->getAnnotationMode();
    if (mode != Polytempo_GraphicsAnnotationManager::Standard && mode != Polytempo_GraphicsAnnotationManager::Edit)
        return;

    Polytempo_GraphicsViewRegion::displayedImage* pDisplayedImage = getDisplayedImageAt(mousePosition);

    if (pDisplayedImage == nullptr)
        return;

    if (status == Selected)
    {
        handleEndEditAccept();
    }

    if (!Polytempo_GraphicsAnnotationManager::getInstance()->trySetAnnotationPending(this))
        return;

    Point<float> imageCoordinates = getImageCoordinates(pDisplayedImage, mousePosition);

    bool existingAnnotationHit = tryGetExistingAnnotation(pDisplayedImage, imageCoordinates);

    if (mode == Polytempo_GraphicsAnnotationManager::Edit)
    {
        if (existingAnnotationHit)
        {
            temporaryAnnotationMove = temporaryAnnotation;
            temporaryAnnotationMove.m_color = Colours::black;
            temporaryAnnotationMove.m_lineWeight = 2.0f;
            setStatus(DragDrop);
            Polytempo_GraphicsPalette::getInstance()->setVisible(true);
        }
        else
        {
            handleEndEditCancel();
            setStatus(Default);
        }
    }
    else
    {
        if (!existingAnnotationHit)
        {
            temporaryAnnotation.clear();
            temporaryAnnotation.m_id = Uuid();
            temporaryAnnotation.m_imageId = pDisplayedImage->imageId;
            temporaryAnnotation.m_referencePoint = imageCoordinates;
            temporaryAnnotation.m_color = Polytempo_GraphicsPalette::getInstance()->getLastColour();
            temporaryAnnotation.m_fontSize = Polytempo_GraphicsPalette::getInstance()->getLastTextSize();
            temporaryAnnotation.m_lineWeight = Polytempo_GraphicsPalette::getInstance()->getLastLineWeight();
            temporaryAnnotation.m_displayedImage = pDisplayedImage;
        }

        if (!temporaryAnnotation.m_freeHandPath.isEmpty())
            temporaryAnnotation.m_freeHandPath.startNewSubPath(imageCoordinates);

        setStatus(FreehandEditing);
    }

    repaint();
    grabKeyboardFocus();
}

void Polytempo_GraphicsAnnotationLayer::handleFreeHandPainting(const Point<int>& mousePosition)
{
    if (temporaryAnnotation.m_freeHandPath.isEmpty() || lastPathPoint.getDistanceFrom(mousePosition) > 1)
    {
        Point<float> imagePt = getImageCoordinates(temporaryAnnotation.m_displayedImage, mousePosition);
        float x = imagePt.getX();
        float y = imagePt.getY();

        if (temporaryAnnotation.m_freeHandPath.isEmpty())
        {
            temporaryAnnotation.m_freeHandPath.addLineSegment(Line<float>(x, y, x, y), Polytempo_GraphicsPalette::getInstance()->getLastLineWeight());
        }
        else
        {
            temporaryAnnotation.m_freeHandPath.lineTo(x, y);
        }

        lastPathPoint = mousePosition;
        repaint();
    }
}

void Polytempo_GraphicsAnnotationLayer::handleEndEditAccept()
{
    if (temporaryAnnotation.m_freeHandPath.isEmpty() && temporaryAnnotation.m_text.isEmpty())
        return; // noting to add

    Polytempo_GraphicsAnnotationManager::getInstance()->addAnnotation(temporaryAnnotation);
    handleEndEdit();
}

void Polytempo_GraphicsAnnotationLayer::handleEndEditCancel()
{
    temporaryAnnotation.clear();
    handleEndEdit();
}

void Polytempo_GraphicsAnnotationLayer::handleEndEdit()
{
    setStatus(Default);
    Polytempo_GraphicsPalette::getInstance()->setVisible(false);

    Polytempo_GraphicsAnnotationManager::getInstance()->resetAnnotationPending(this);
    fullUpdateRequired.set(true);
}

void Polytempo_GraphicsAnnotationLayer::handleDeleteSelected()
{
    Polytempo_GraphicsAnnotationManager::getInstance()->removeAnnotation(temporaryAnnotation.m_id);
    handleEndEditCancel();
}

void Polytempo_GraphicsAnnotationLayer::setStatus(Status newStatus)
{
    status = newStatus;

    switch (status)
    {
    case Default:
    case Selected:
        setMouseCursor(MouseCursor::NormalCursor);
        break;

    case FreehandEditing:
        setMouseCursor(MouseCursor::CrosshairCursor);
        break;

    case DragDrop:
        setMouseCursor(MouseCursor::DraggingHandCursor);
        break;
    }
}

void Polytempo_GraphicsAnnotationLayer::mouseDown(const MouseEvent& event)
{
    if (status == Default || status == Selected)
    {
        handleStartEditing(event.getPosition());
    }
    else if (status == FreehandEditing)
    {
        Polytempo_GraphicsViewRegion::displayedImage* pDisplayedImage = getDisplayedImageAt(event.getPosition());
        
        if (pDisplayedImage == nullptr || pDisplayedImage->imageId != temporaryAnnotation.m_imageId || pDisplayedImage != temporaryAnnotation.m_displayedImage)
        {
            handleEndEditAccept();
            handleStartEditing(event.getPosition());
        }
        else
        {
            temporaryAnnotation.m_freeHandPath.startNewSubPath(getImageCoordinates(pDisplayedImage, event.getPosition()));
        }
        stopTimer(TIMER_ID_AUTO_ACCEPT);
    }
}

void Polytempo_GraphicsAnnotationLayer::mouseUp(const MouseEvent& event)
{
    if (status == Default)
    {
        if (event.getLengthOfMousePress() > MIN_MOUSE_DOWN_TIME_MS && event.getDistanceFromDragStart() < 10)
        {
            handleStartEditing(event.getPosition());
        }
    }

    if (status == FreehandEditing)
    {
        Polytempo_GraphicsPalette::getInstance()->setVisible(true);
        grabKeyboardFocus();
        startTimer(TIMER_ID_AUTO_ACCEPT, AUTO_ACCEPT_INTERVAL_MS);
    }
    else if (status == DragDrop)
    {
        temporaryAnnotation.m_freeHandPath = temporaryAnnotationMove.m_freeHandPath;
        temporaryAnnotation.m_referencePoint = temporaryAnnotationMove.m_referencePoint;
        temporaryAnnotationMove.clear();
        Polytempo_GraphicsAnnotationManager::getInstance()->addAnnotation(temporaryAnnotation); // immediate commit
        fullUpdateRequired.set(true);
        setStatus(Selected);
        repaint();
    }
}

void Polytempo_GraphicsAnnotationLayer::mouseDrag(const MouseEvent& event)
{
    if (status == FreehandEditing)
    {
        handleFreeHandPainting(event.getPosition());
    }
    else if (status == DragDrop)
    {
        Point<float> imagePt = getImageCoordinates(temporaryAnnotation.m_displayedImage, event.getPosition());
        if (temporaryAnnotation.m_displayedImage->imageRect.contains(int(imagePt.getX()), int(imagePt.getY())))
        {
            float x = imagePt.getX();
            float y = imagePt.getY();

            temporaryAnnotationMove.m_referencePoint.setXY(x, y);
            temporaryAnnotationMove.m_freeHandPath = temporaryAnnotation.m_freeHandPath;
            temporaryAnnotationMove.m_freeHandPath.applyTransform(AffineTransform::translation(x - temporaryAnnotation.m_referencePoint.getX(), y - temporaryAnnotation.m_referencePoint.getY()));
            repaint();
        }
    }
}

void Polytempo_GraphicsAnnotationLayer::mouseDoubleClick(const MouseEvent& event)
{
    if (status == Default)
    {
        handleStartEditing(event.getPosition());
    }
}

void Polytempo_GraphicsAnnotationLayer::timerCallback(int timerID)
{
    switch (timerID)
    {
    case TIMER_ID_REPAINT:
        stopTimer(timerID);
        repaint();
        startTimer(timerID, MIN_INTERVAL_BETWEEN_REPAINTS_MS);
        break;
    case TIMER_ID_AUTO_ACCEPT:
        stopTimer(timerID);
        if (status == FreehandEditing)
            handleEndEditAccept();
        break;
    default:
        stopTimer(timerID);
    }
}

bool Polytempo_GraphicsAnnotationLayer::keyPressed(const KeyPress& key)
{
    if (status != Default && key.isValid() && !key.getModifiers().isCtrlDown())
    {
        if (key == KeyPress::escapeKey)
            handleEndEditCancel();
        else if (key == KeyPress::returnKey)
            handleEndEditAccept();
        else if (key == KeyPress::backspaceKey)
        {
            if (temporaryAnnotation.m_text.length() == 0)
                return false; // handle as delete command in NetworkMenuBarModel

            temporaryAnnotation.m_text = temporaryAnnotation.m_text.dropLastCharacters(1);
            startTimer(TIMER_ID_AUTO_ACCEPT, AUTO_ACCEPT_INTERVAL_MS);
            fullUpdateRequired.set(true);
        }
        else if (key == KeyPress::deleteKey)
        {
            return false; // handle as delete command in NetworkMenuBarModel
        }
        else
        {
            juce_wchar character = key.getTextCharacter();
            String str;
            str += character;
            if (!str.isEmpty())
                temporaryAnnotation.m_text.append(str, 1);
            startTimer(TIMER_ID_AUTO_ACCEPT, AUTO_ACCEPT_INTERVAL_MS);
            repaint();
        }
        return true;
    }

    return false;
}

int Polytempo_GraphicsAnnotationLayer::getTemporaryFontSize() const
{
    return int(temporaryAnnotation.m_fontSize);
}

void Polytempo_GraphicsAnnotationLayer::setTemporaryColor(Colour colour)
{
    if (status != Default)
    {
        temporaryAnnotation.m_color = colour.withAlpha(uint8(jmax(uint8(50), colour.getAlpha())));

        if (status == Selected)
        {
            // direct commit if in edit mode
            Polytempo_GraphicsAnnotationManager::getInstance()->addAnnotation(temporaryAnnotation);
        }

        fullUpdateRequired.set(true);
    }
}

void Polytempo_GraphicsAnnotationLayer::stopAutoAccept()
{
    stopTimer(TIMER_ID_AUTO_ACCEPT);
}

void Polytempo_GraphicsAnnotationLayer::restartAutoAccept()
{
    if (status == FreehandEditing)
        startTimer(TIMER_ID_AUTO_ACCEPT, AUTO_ACCEPT_INTERVAL_MS);
}

Colour Polytempo_GraphicsAnnotationLayer::getTemporaryColor() const
{
    return temporaryAnnotation.m_color;
}

float Polytempo_GraphicsAnnotationLayer::getTemporaryLineWeight() const
{
    return temporaryAnnotation.m_lineWeight;
}

void Polytempo_GraphicsAnnotationLayer::setTemporaryLineWeight(float lineWeight)
{
    temporaryAnnotation.m_lineWeight = lineWeight;
    if (status == Selected)
    {
        // direct commit if in edit mode
        Polytempo_GraphicsAnnotationManager::getInstance()->addAnnotation(temporaryAnnotation);
    }

    fullUpdateRequired.set(true);
}

void Polytempo_GraphicsAnnotationLayer::setTemporaryFontSize(float fontSize)
{
    temporaryAnnotation.m_fontSize = fontSize;
    if (status == Selected)
    {
        // direct commit if in edit mode
        Polytempo_GraphicsAnnotationManager::getInstance()->addAnnotation(temporaryAnnotation);
    }

    fullUpdateRequired.set(true);
}

void Polytempo_GraphicsAnnotationLayer::changeListenerCallback(ChangeBroadcaster* source)
{
    if (source == Polytempo_GraphicsAnnotationManager::getInstance())
    {
        Polytempo_GraphicsAnnotationManager::eAnnotationMode currentMode = Polytempo_GraphicsAnnotationManager::getInstance()->getAnnotationMode();
        if (currentMode != lastAnnotationMode && Polytempo_GraphicsAnnotationManager::getInstance()->isAnnotationPending())
            handleEndEditAccept();

        lastAnnotationMode = currentMode;

        fullUpdateRequired.set(true);
    }
}

bool Polytempo_GraphicsAnnotationLayer::tryGetExistingAnnotation(Polytempo_GraphicsViewRegion::displayedImage* img, Point<float> point)
{
    if (Polytempo_GraphicsAnnotationManager::getInstance()->getAnchorFlag())
    {
        Polytempo_GraphicsAnnotation* nearestAnnotation = nullptr;
        float nearestAnnotationDistance = FLT_MAX;

        OwnedArray<Polytempo_GraphicsAnnotation> annotations;
        Polytempo_GraphicsAnnotationManager::getInstance()->getAnnotationsForImage(img->imageId, &annotations);

        // find nearest annotation in anchor mode
        for (int i = 0; i < annotations.size(); i++)
        {
            float dist = annotations[i]->m_referencePoint.getDistanceFrom(Point<float>(point.getX(), point.getY()));
            if (dist < nearestAnnotationDistance)
            {
                nearestAnnotation = annotations[i];
                nearestAnnotationDistance = dist;
            }
        }

        if (nearestAnnotationDistance < 15.0f)
        {
            if (Polytempo_GraphicsAnnotationManager::getInstance()->getAnnotation(nearestAnnotation->m_id, &temporaryAnnotation))
            {
                temporaryAnnotation.m_displayedImage = img;
                return true;
            }

            return false;
        }

        return false;
    }

    return false;
}
