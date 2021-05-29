#include "JuceHeader.h"
#include "Polytempo_GraphicsAnnotationLayer.h"
#include "Polytempo_GraphicsAnnotationManager.h"
#include "Polytempo_GraphicsPalette.h"
#include <float.h>

Polytempo_GraphicsAnnotationLayer::Polytempo_GraphicsAnnotationLayer(HashMap<String, Polytempo_GraphicsViewRegion*>* pRegionMap)
{
    this->pRegionMap = pRegionMap;
    status = Default;
    setAlwaysOnTop(true);
    addKeyListener(this);
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

    if ((status == FreehandEditing || status == Selected) && temporaryAnnotation.displayedImage != nullptr)
    {
        g.addTransform(temporaryAnnotation.displayedImage->imageToScreen);
        paintAnnotation(g, &temporaryAnnotation, true, Colours::red);
    }
    else if (status == DragDrop && temporaryAnnotationMove.displayedImage != nullptr)
    {
        g.addTransform(temporaryAnnotation.displayedImage->imageToScreen);
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

    HashMap<String, Polytempo_GraphicsViewRegion*>::Iterator it1(*pRegionMap);
    while (it1.next())
    {
        OwnedArray<Polytempo_GraphicsAnnotation> annotations;
        for(Polytempo_GraphicsViewRegion::displayedImage img : it1.getValue()->getDisplayedImages())
        {
            Polytempo_GraphicsAnnotationManager::getInstance()->getAnnotationsForImage(img.imageId, &annotations);
            for (Polytempo_GraphicsAnnotation* annotation : annotations)
            {
                if (img.imageId == annotation->imageId &&
                    img.imageRect.contains(int(annotation->referencePoint.x), int(annotation->referencePoint.y)) &&
                    (status != FreehandEditing || temporaryAnnotation.id != annotation->id))
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
    strokeType.setStrokeThickness(annotation->lineWeight);

    g.setColour(annotation->color);
    if (!annotation->freeHandPath.isEmpty())
        g.strokePath(annotation->freeHandPath, strokeType);

    if (!annotation->text.isEmpty())
    {
        g.setFont(annotation->fontSize);
        g.drawSingleLineText(annotation->text, int(annotation->referencePoint.getX()) + 10, int(annotation->referencePoint.getY()));
    }

    if (anchorFlag)
    {
        g.setColour(anchorColor);
        g.fillRoundedRectangle(annotation->referencePoint.getX() - 8.0f, annotation->referencePoint.getY() - 8.0f, 16.0f, 16.0f, 4.0f);
    }
}

void Polytempo_GraphicsAnnotationLayer::requireUpdate()
{
    fullUpdateRequired.set(true);
}

Polytempo_GraphicsViewRegion::displayedImage* Polytempo_GraphicsAnnotationLayer::getDisplayedImageAt(Point<int> mouse) const
{
    HashMap<String, Polytempo_GraphicsViewRegion*>::Iterator it1(*pRegionMap);
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
            temporaryAnnotationMove.color = Colours::black;
            temporaryAnnotationMove.lineWeight = 2.0f;
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
            temporaryAnnotation.id = Uuid();
            temporaryAnnotation.imageId = pDisplayedImage->imageId;
            temporaryAnnotation.referencePoint = imageCoordinates;
            temporaryAnnotation.color = Polytempo_GraphicsPalette::getInstance()->getLastColour();
            temporaryAnnotation.fontSize = Polytempo_GraphicsPalette::getInstance()->getLastTextSize();
            temporaryAnnotation.lineWeight = Polytempo_GraphicsPalette::getInstance()->getLastLineWeight();
            temporaryAnnotation.displayedImage = pDisplayedImage;
        }

        if (!temporaryAnnotation.freeHandPath.isEmpty())
            temporaryAnnotation.freeHandPath.startNewSubPath(imageCoordinates);

        setStatus(FreehandEditing);
    }

    repaint();
    grabKeyboardFocus();
}

void Polytempo_GraphicsAnnotationLayer::handleFreeHandPainting(const Point<int>& mousePosition)
{
    if (temporaryAnnotation.freeHandPath.isEmpty() || lastPathPoint.getDistanceFrom(mousePosition) > 1)
    {
        Point<float> imagePt = getImageCoordinates(temporaryAnnotation.displayedImage, mousePosition);
        float x = imagePt.getX();
        float y = imagePt.getY();

        if (temporaryAnnotation.freeHandPath.isEmpty())
        {
            temporaryAnnotation.freeHandPath.addLineSegment(Line<float>(x, y, x, y), Polytempo_GraphicsPalette::getInstance()->getLastLineWeight());
        }
        else
        {
            temporaryAnnotation.freeHandPath.lineTo(x, y);
        }

        lastPathPoint = mousePosition;
        repaint();
    }
}

void Polytempo_GraphicsAnnotationLayer::handleEndEditAccept()
{
    if (temporaryAnnotation.freeHandPath.isEmpty() && temporaryAnnotation.text.isEmpty())
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
    Polytempo_GraphicsAnnotationManager::getInstance()->removeAnnotation(temporaryAnnotation.id);
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
        
        if (pDisplayedImage == nullptr || pDisplayedImage->imageId != temporaryAnnotation.imageId || pDisplayedImage != temporaryAnnotation.displayedImage)
        {
            handleEndEditAccept();
            handleStartEditing(event.getPosition());
        }
        else
        {
            temporaryAnnotation.freeHandPath.startNewSubPath(getImageCoordinates(pDisplayedImage, event.getPosition()));
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
        temporaryAnnotation.freeHandPath = temporaryAnnotationMove.freeHandPath;
        temporaryAnnotation.referencePoint = temporaryAnnotationMove.referencePoint;
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
        Point<float> imagePt = getImageCoordinates(temporaryAnnotation.displayedImage, event.getPosition());
        if (temporaryAnnotation.displayedImage->imageRect.contains(int(imagePt.getX()), int(imagePt.getY())))
        {
            float x = imagePt.getX();
            float y = imagePt.getY();

            temporaryAnnotationMove.referencePoint.setXY(x, y);
            temporaryAnnotationMove.freeHandPath = temporaryAnnotation.freeHandPath;
            temporaryAnnotationMove.freeHandPath.applyTransform(AffineTransform::translation(x - temporaryAnnotation.referencePoint.getX(), y - temporaryAnnotation.referencePoint.getY()));
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

bool Polytempo_GraphicsAnnotationLayer::keyPressed(const KeyPress& key, Component* /*originatingComponent*/)
{
    if (status != Default && key.isValid() && !key.getModifiers().isCtrlDown())
    {
        if (key == KeyPress::escapeKey)
            handleEndEditCancel();
        else if (key == KeyPress::returnKey)
            handleEndEditAccept();
        else if (key == KeyPress::backspaceKey)
        {
            if (temporaryAnnotation.text.length() == 0)
                return false; // handle as delete command in NetworkMenuBarModel

            temporaryAnnotation.text = temporaryAnnotation.text.dropLastCharacters(1);
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
                temporaryAnnotation.text.append(str, 1);
            startTimer(TIMER_ID_AUTO_ACCEPT, AUTO_ACCEPT_INTERVAL_MS);
            repaint();
        }
        return true;
    }

    return false;
}

int Polytempo_GraphicsAnnotationLayer::getTemporaryFontSize() const
{
    return int(temporaryAnnotation.fontSize);
}

void Polytempo_GraphicsAnnotationLayer::setTemporaryColor(Colour colour)
{
    if (status != Default)
    {
        temporaryAnnotation.color = colour.withAlpha(uint8(jmax(uint8(50), colour.getAlpha())));

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
    return temporaryAnnotation.color;
}

float Polytempo_GraphicsAnnotationLayer::getTemporaryLineWeight() const
{
    return temporaryAnnotation.lineWeight;
}

void Polytempo_GraphicsAnnotationLayer::setTemporaryLineWeight(float lineWeight)
{
    temporaryAnnotation.lineWeight = lineWeight;
    if (status == Selected)
    {
        // direct commit if in edit mode
        Polytempo_GraphicsAnnotationManager::getInstance()->addAnnotation(temporaryAnnotation);
    }

    fullUpdateRequired.set(true);
}

void Polytempo_GraphicsAnnotationLayer::setTemporaryFontSize(float fontSize)
{
    temporaryAnnotation.fontSize = fontSize;
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
            float dist = annotations[i]->referencePoint.getDistanceFrom(Point<float>(point.getX(), point.getY()));
            if (dist < nearestAnnotationDistance)
            {
                nearestAnnotation = annotations[i];
                nearestAnnotationDistance = dist;
            }
        }

        if (nearestAnnotationDistance < 15.0f)
        {
            if (Polytempo_GraphicsAnnotationManager::getInstance()->getAnnotation(nearestAnnotation->id, &temporaryAnnotation))
            {
                temporaryAnnotation.displayedImage = img;
                return true;
            }

            return false;
        }

        return false;
    }

    return false;
}
