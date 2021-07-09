#include "Polytempo_PageEditorComponent.h"
#include "../../Preferences/Polytempo_StoredPreferences.h"
#include "../../Application/PolytempoNetwork/Polytempo_NetworkApplication.h"
#include "Polytempo_PageEditorView.h"

Polytempo_PageEditorComponent::Polytempo_PageEditorComponent()
{
    image = nullptr;

    for (int i = 0; i < 4; i++)
    {
        DragHandle* dh = new DragHandle(this);
        dh->setConstraint(i % 2 == 0 ? 2 : 1);
        dragHandles.add(dh);
        addChildComponent(dh);
    }

    zoomFactor = (float)Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("zoom");
    Polytempo_StoredPreferences::getInstance()->getProps().addChangeListener(this);
}

Polytempo_PageEditorComponent::~Polytempo_PageEditorComponent()
{
    deleteAllChildren();
}

void Polytempo_PageEditorComponent::update()
{
}

void Polytempo_PageEditorComponent::paint(Graphics& g)
{
    if (image != nullptr)
    {
        g.drawImage(*image,
                    0, 0, (int)(image->getWidth() * zoomFactor), (int)(image->getHeight() * zoomFactor),
                    0, 0, image->getWidth(), image->getHeight());

        if (!sectionRect.isEmpty())
        {
            g.setColour(Colours::orangered);

            g.drawRect(dragHandles.getUnchecked(3)->getCentreX(),
                       dragHandles.getUnchecked(0)->getCentreY(),
                       dragHandles.getUnchecked(1)->getCentreX() - dragHandles.getUnchecked(3)->getCentreX(),
                       dragHandles.getUnchecked(2)->getCentreY() - dragHandles.getUnchecked(0)->getCentreY(),
                       1.0);
        }
    }
}

void Polytempo_PageEditorComponent::resized()
{
    if (image && image->isValid())
    {
        Rectangle<int> r = getBounds();
        setBounds(r.getX(), r.getY(), (int)(image->getWidth() * zoomFactor), (int)(image->getHeight() * zoomFactor));

        if (dragHandles.getUnchecked(0)->isVisible())
        {
            dragHandles.getUnchecked(0)->setCentrePosition((int)((sectionRect.getX() + sectionRect.getWidth() * 0.5) * image->getWidth() * zoomFactor), (int)(sectionRect.getY() * image->getHeight() * zoomFactor));

            dragHandles.getUnchecked(1)->setCentrePosition((int)((sectionRect.getX() + sectionRect.getWidth()) * image->getWidth() * zoomFactor), (int)((sectionRect.getY() + sectionRect.getHeight() * 0.5) * image->getHeight() * zoomFactor));

            dragHandles.getUnchecked(2)->setCentrePosition((int)((sectionRect.getX() + sectionRect.getWidth() * 0.5) * image->getWidth() * zoomFactor), (int)((sectionRect.getY() + sectionRect.getHeight()) * image->getHeight() * zoomFactor));

            dragHandles.getUnchecked(3)->setCentrePosition((int)(sectionRect.getX() * image->getWidth() * zoomFactor), (int)((sectionRect.getY() + sectionRect.getHeight() * 0.5) * image->getHeight() * zoomFactor));
        }
    }
    else
        setBounds(0, 0, 0, 0);
}

void Polytempo_PageEditorComponent::setImage(Image* img)
{
    image = img;
    resized();
}

void Polytempo_PageEditorComponent::setSectionRect(Rectangle<float> r)
{
    sectionRect = r;

    if (sectionRect.getWidth() == 0 || sectionRect.getHeight() == 0)
    {
        for (int i = 0; i < 4; i++) { dragHandles.getUnchecked(i)->setVisible(false); }
    }
    else
    {
        for (int i = 0; i < 4; i++)
        {
            dragHandles.getUnchecked(i)->setVisible(true);
            Rectangle<int> nr = Rectangle<int>(0,0,(int)(image->getWidth() * zoomFactor), (int)(image->getHeight() * zoomFactor));
            dragHandles.getUnchecked(i)->setBoundsConstraint(nr);
        }
        resized();
    }
}

void Polytempo_PageEditorComponent::setEditedEvent(Polytempo_Event* event)
{
    if (event->getType() == eventType_AddSection) editedEvent = event;

    Polytempo_NetworkApplication* const app = dynamic_cast<Polytempo_NetworkApplication*>(JUCEApplication::getInstance());
    score = app->getScore();
}

void Polytempo_PageEditorComponent::draggingSessionEnded()
{
}

void Polytempo_PageEditorComponent::positionChanged(DragHandle* handle)
{
    if (handle != dragHandles.getUnchecked(0))
        dragHandles.getUnchecked(0)->setCentrePosition((int)(dragHandles.getUnchecked(3)->getCentreX() + (dragHandles.getUnchecked(1)->getCentreX() - dragHandles.getUnchecked(3)->getCentreX()) * 0.5), (int)(dragHandles.getUnchecked(0)->getCentreY()));

    if (handle != dragHandles.getUnchecked(1))
        dragHandles.getUnchecked(1)->setCentrePosition((int)(dragHandles.getUnchecked(1)->getCentreX()), (int)(dragHandles.getUnchecked(2)->getCentreY() + (dragHandles.getUnchecked(0)->getCentreY() - dragHandles.getUnchecked(2)->getCentreY()) * 0.5));

    if (handle != dragHandles.getUnchecked(2))
        dragHandles.getUnchecked(2)->setCentrePosition((int)(dragHandles.getUnchecked(3)->getCentreX() + (dragHandles.getUnchecked(1)->getCentreX() - dragHandles.getUnchecked(3)->getCentreX()) * 0.5), (int)(dragHandles.getUnchecked(2)->getCentreY()));

    if (handle != dragHandles.getUnchecked(3))
        dragHandles.getUnchecked(3)->setCentrePosition((int)(dragHandles.getUnchecked(3)->getCentreX()), (int)(dragHandles.getUnchecked(2)->getCentreY() + (dragHandles.getUnchecked(0)->getCentreY() - dragHandles.getUnchecked(2)->getCentreY()) * 0.5));

    Array<var> r;
    r.add((dragHandles.getUnchecked(3)->getCentreX()) / image->getWidth() / zoomFactor);
    r.add(dragHandles.getUnchecked(0)->getCentreY() / image->getHeight() / zoomFactor);
    r.add((dragHandles.getUnchecked(1)->getCentreX() - dragHandles.getUnchecked(3)->getCentreX()) / image->getWidth() / zoomFactor);
    r.add((dragHandles.getUnchecked(2)->getCentreY() - dragHandles.getUnchecked(0)->getCentreY()) / image->getHeight() / zoomFactor);

    if (editedEvent)
    {
        editedEvent->setProperty(eventPropertyString_Rect, r);
        score->setDirty();
    }

    // update everything
    Component* parent = getParentComponent()->getParentComponent()->getParentComponent();
    ((Polytempo_PageEditorView*)parent)->update();
}

void Polytempo_PageEditorComponent::changeListenerCallback(ChangeBroadcaster*)
{
    if (!isVisible()) return;

    if (zoomFactor != Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("zoom"))
    {
        zoomFactor = (float)Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("zoom");
        resized();
    }
}
