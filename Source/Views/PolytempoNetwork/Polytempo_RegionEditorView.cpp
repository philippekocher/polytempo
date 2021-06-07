#include "Polytempo_RegionEditorView.h"
#include "../../Application/PolytempoNetwork/Polytempo_NetworkApplication.h"

#define COORDINATE_VIEW_WIDTH 150

Polytempo_RegionEditorView::Polytempo_RegionEditorView()
{
    for (int i = 0; i < 4; i++)
    {
        DragHandle* dh = new DragHandle(this);
        dh->setConstraint(i % 2 == 0 ? 2 : 1);
        dragHandles.add(dh);
        addChildComponent(dh);
    }

    addAndMakeVisible(relativePositionLabel = new Label(String(), "Region --"));
    relativePositionLabel->setFont(Font(15.0f, Font::plain));

    addAndMakeVisible(xTextbox = new Polytempo_Textbox("Left"));
    xTextbox->setFont(Font(16.0f, Font::plain));
    xTextbox->addListener(this);

    addAndMakeVisible(yTextbox = new Polytempo_Textbox("Top"));
    yTextbox->setFont(Font(16.0f, Font::plain));
    yTextbox->addListener(this);

    addAndMakeVisible(wTextbox = new Polytempo_Textbox("Width"));
    wTextbox->setFont(Font(16.0f, Font::plain));
    wTextbox->addListener(this);

    addAndMakeVisible(hTextbox = new Polytempo_Textbox("Height"));
    hTextbox->setFont(Font(16.0f, Font::plain));
    hTextbox->addListener(this);
}

Polytempo_RegionEditorView::~Polytempo_RegionEditorView()
{
    deleteAllChildren();
}

void Polytempo_RegionEditorView::paint(Graphics& g)
{
    g.fillAll(Colours::white);

    Rectangle<int> r(getLocalBounds());

    g.setColour(Colour(245, 245, 245));
    g.fillRect(r.removeFromRight(COORDINATE_VIEW_WIDTH));
    g.setColour(Colours::grey);
    g.drawVerticalLine(getWidth() - COORDINATE_VIEW_WIDTH, 0.0f, (float)getHeight());

    int width = r.getWidth();
    int height = getHeight();
    int index = 0;

    if (selectedRegionID < addRegionEvents.size())
        for (int i = 0; i < 4; i++) dragHandles.getUnchecked(i)->setVisible(true);
    else
        for (int i = 0; i < 4; i++) dragHandles.getUnchecked(i)->setVisible(false);

    for (Polytempo_Event* addRegionEvent : addRegionEvents)
    {
        Array<var> rect = *addRegionEvent->getProperty(eventPropertyString_Rect).getArray();

        Rectangle<int> bounds = Rectangle<int>(int(width * float(rect[0])), int(height * float(rect[1])), int(width * float(rect[2])), int(height * float(rect[3])));

        if (index++ == selectedRegionID)
        {
            dragHandles.getUnchecked(0)->setCentrePosition(int((bounds.getX() + bounds.getWidth() * 0.5f)), int(bounds.getY()));

            dragHandles.getUnchecked(1)->setCentrePosition(int((bounds.getX() + bounds.getWidth())), int((bounds.getY() + bounds.getHeight() * 0.5f)));

            dragHandles.getUnchecked(2)->setCentrePosition(int((bounds.getX() + bounds.getWidth() * 0.5f)), int((bounds.getY() + bounds.getHeight())));

            dragHandles.getUnchecked(3)->setCentrePosition(int(bounds.getX()), int((bounds.getY() + bounds.getHeight() * 0.5f)));

            if (!bounds.isEmpty())
            {
                g.setColour(Colours::blue.withAlpha(0.05f));
                g.fillRect(bounds);
                g.setColour(Colours::blue);
                g.drawRect(bounds, 2);

                if (bounds.getHeight() < bounds.getWidth() * 2)
                    g.setFont(float(bounds.getHeight()));
                else
                    g.setFont(bounds.getWidth() * 2.0f);
                g.setColour(Colours::blue.withAlpha(0.2f));
                g.drawFittedText(addRegionEvent->getProperty(eventPropertyString_RegionID).toString(), bounds, Justification::centred, 1, 0.1f);
            }
        }
        else
        {
            g.setColour(Colours::blue.withAlpha(0.7f));
            g.drawRect(width * float(rect[0]) + 1, height * float(rect[1]) + 1, width * float(rect[2]) - 2, height * float(rect[3]) - 2, 1.0f);

            if (bounds.getHeight() < bounds.getWidth() * 2)
                g.setFont(float(bounds.getHeight()));
            else
                g.setFont(bounds.getWidth() * 2.0f);
            g.setColour(Colours::blue.withAlpha(0.05f));
            g.drawFittedText(addRegionEvent->getProperty(eventPropertyString_RegionID).toString(), bounds, Justification::centred, 1, 0.1f);
        }
    }
}

void Polytempo_RegionEditorView::resized()
{
    relativePositionLabel->setBounds(getWidth() - COORDINATE_VIEW_WIDTH + 5, 0, COORDINATE_VIEW_WIDTH - 20, 34);

    xTextbox->setBounds(getWidth() - COORDINATE_VIEW_WIDTH + 10, 50, 55, 26);
    yTextbox->setBounds((int)(getWidth() - COORDINATE_VIEW_WIDTH * 0.5 + 10), 50, 55, 26);
    wTextbox->setBounds(getWidth() - COORDINATE_VIEW_WIDTH + 10, 95, 55, 26);
    hTextbox->setBounds((int)(getWidth() - COORDINATE_VIEW_WIDTH * 0.5 + 10), 95, 55, 26);

    Rectangle<int> r(getLocalBounds());
    r.removeFromRight(COORDINATE_VIEW_WIDTH);
    dragHandles[1]->setBoundsConstraint(r);
}

void Polytempo_RegionEditorView::refresh()
{
    Polytempo_NetworkApplication* const app = dynamic_cast<Polytempo_NetworkApplication*>(JUCEApplication::getInstance());
    score = app->getScore();

    if (score == nullptr) return;

    addRegionEvents = score->getEvents(eventType_AddRegion);
    setSelectedRegionID(0);
    repaint();
}

bool Polytempo_RegionEditorView::keyPressed(const KeyPress&)
{
    return false;
}

void Polytempo_RegionEditorView::mouseDown(const MouseEvent& event)
{
    int i = 0;
    int width = getWidth() - COORDINATE_VIEW_WIDTH;
    int height = getHeight();

    for (Polytempo_Event* addRegionEvent : addRegionEvents)
    {
        Array<var> r = *addRegionEvent->getProperty(eventPropertyString_Rect).getArray();
        Rectangle<int> bounds = Rectangle<int>(int(width * float(r[0])), int(height * float(r[1])), int(width * float(r[2])), int(height * float(r[3])));

        if (bounds.contains(event.getPosition()))
            break;

        i++;
    }
    if (selectedRegionID != i)
    {
        setSelectedRegionID(i);
        repaint();
    }
}

void Polytempo_RegionEditorView::addRegion()
{
    // find new id
    int newID = 0;
    bool success = false;

    while (!success)
    {
        newID++;
        success = true;
        for (Polytempo_Event* addRegionEvent : addRegionEvents)
        {
            if (addRegionEvent->getProperty(eventPropertyString_RegionID).equals(var(newID)))
            {
                success = false;
                break;
            }
        }
    }

    Polytempo_Event* event = Polytempo_Event::makeEvent(eventType_AddRegion);
    event->setProperty(eventPropertyString_Rect, Polytempo_Event::defaultRectangle());
    event->setProperty(eventPropertyString_RegionID, var(newID));

    score->addEvent(event, true);
    score->setDirty();

    addRegionEvents.add(event);
    setSelectedRegionID(addRegionEvents.size() - 1);

    repaint();
}

void Polytempo_RegionEditorView::draggingSessionEnded()
{
}

void Polytempo_RegionEditorView::positionChanged(DragHandle* handle)
{
    int width = getWidth() - COORDINATE_VIEW_WIDTH;
    int height = getHeight();

    if (handle != dragHandles.getUnchecked(0))
        dragHandles.getUnchecked(0)->setCentrePosition((int)(dragHandles.getUnchecked(3)->getCentreX() + (dragHandles.getUnchecked(1)->getCentreX() - dragHandles.getUnchecked(3)->getCentreX()) * 0.5), (int)(dragHandles.getUnchecked(0)->getCentreY()));

    if (handle != dragHandles.getUnchecked(1))
        dragHandles.getUnchecked(1)->setCentrePosition((int)(dragHandles.getUnchecked(1)->getCentreX()), (int)(dragHandles.getUnchecked(2)->getCentreY() + (dragHandles.getUnchecked(0)->getCentreY() - dragHandles.getUnchecked(2)->getCentreY()) * 0.5));

    if (handle != dragHandles.getUnchecked(2))
        dragHandles.getUnchecked(2)->setCentrePosition((int)(dragHandles.getUnchecked(3)->getCentreX() + (dragHandles.getUnchecked(1)->getCentreX() - dragHandles.getUnchecked(3)->getCentreX()) * 0.5), (int)(dragHandles.getUnchecked(2)->getCentreY()));

    if (handle != dragHandles.getUnchecked(3))
        dragHandles.getUnchecked(3)->setCentrePosition((int)(dragHandles.getUnchecked(3)->getCentreX()), (int)(dragHandles.getUnchecked(2)->getCentreY() + (dragHandles.getUnchecked(0)->getCentreY() - dragHandles.getUnchecked(2)->getCentreY()) * 0.5));

    Array<var> r;
    r.add(dragHandles.getUnchecked(3)->getCentreX() / width);
    r.add(dragHandles.getUnchecked(0)->getCentreY() / height);
    r.add((dragHandles.getUnchecked(1)->getCentreX() - dragHandles.getUnchecked(3)->getCentreX()) / width);
    r.add((dragHandles.getUnchecked(2)->getCentreY() - dragHandles.getUnchecked(0)->getCentreY()) / height);

    updateSelectedRegion(r);
    score->setDirty();

    repaint();
}

void Polytempo_RegionEditorView::editorShown(Label*, TextEditor&)
{
}

void Polytempo_RegionEditorView::labelTextChanged(Label* label)
{
    if (label == xTextbox || label == yTextbox || label == wTextbox || label == hTextbox)
    {
        Array<var> r = *addRegionEvents.getUnchecked(selectedRegionID)->getProperty(eventPropertyString_Rect).getArray();

        float num = label->getText().getFloatValue();
        num = num < 0.0f ? 0.0f : num > 1.0f ? 1.0f : num;

        if (label == xTextbox)
        {
            r.set(0, num);
            if (num + float(r[2]) > 1.0f) r.set(2, 1.0f - num);
        }
        else if (label == yTextbox) r.set(1, num);
        else if (label == wTextbox)
        {
            r.set(2, num);
            if (float(r[0]) + num > 1.0f) r.set(2, 1.0f - float(r[0]));
        }
        else if (label == hTextbox) r.set(3, num);

        updateSelectedRegion(r);
        score->setDirty();

        repaint();
    }
}

void Polytempo_RegionEditorView::eventNotification(Polytempo_Event* event)
{
    if (event->getType() == eventType_Ready && isVisible())
    {
        refresh();
    }
}

void Polytempo_RegionEditorView::setSelectedRegionID(int id)
{
    selectedRegionID = id;

    if (selectedRegionID < addRegionEvents.size())
    {
        if (!addRegionEvents.getUnchecked(selectedRegionID)->hasProperty(eventPropertyString_Rect))
            addRegionEvents.getUnchecked(selectedRegionID)->setProperty(eventPropertyString_Rect, Polytempo_Event::defaultRectangle());
        
        Array<var> r = *addRegionEvents.getUnchecked(selectedRegionID)->getProperty(eventPropertyString_Rect).getArray();

        xTextbox->setFloat(r[0], dontSendNotification);
        yTextbox->setFloat(r[1], dontSendNotification);
        wTextbox->setFloat(r[2], dontSendNotification);
        hTextbox->setFloat(r[3], dontSendNotification);
        relativePositionLabel->setText("Region " + addRegionEvents.getUnchecked(selectedRegionID)->getProperty(eventPropertyString_RegionID).toString(), dontSendNotification);
    }
    else
    {
        xTextbox->reset();
        yTextbox->reset();
        wTextbox->reset();
        hTextbox->reset();
        relativePositionLabel->setText("Region --", dontSendNotification);
    }
}

void Polytempo_RegionEditorView::updateSelectedRegion(Array<var> r)
{
    addRegionEvents.getUnchecked(selectedRegionID)->setProperty(eventPropertyString_Rect, r);

    xTextbox->setFloat(r[0], dontSendNotification);
    yTextbox->setFloat(r[1], dontSendNotification);
    wTextbox->setFloat(r[2], dontSendNotification);
    hTextbox->setFloat(r[3], dontSendNotification);
}
