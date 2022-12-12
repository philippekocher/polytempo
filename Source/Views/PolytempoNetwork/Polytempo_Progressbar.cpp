#include "Polytempo_Progressbar.h"
#include "Polytempo_GraphicsViewRegion.h"

Polytempo_Progressbar::Polytempo_Progressbar()
{
    setOpaque(false);
    elapsedTime = 0;
}

Polytempo_Progressbar::~Polytempo_Progressbar()
{
}

void Polytempo_Progressbar::paint(Graphics& g)
{
    Rectangle<int> bounds = getLocalBounds();

    g.setColour(Colours::white);
    g.fillRect(bounds);

    bounds.reduce(5,5);

    // text above
    if (text->isNotEmpty())
    {
        int textHeight = bounds.getHeight() > 54 ? 27 : bounds.getHeight() / 2;
        g.setColour(Colours::black);
        g.setFont(24.0f);
        g.drawFittedText(*text,
                         bounds.removeFromTop(textHeight),
                         Justification::topLeft, 1);
    }

    // frame
    g.setColour(Colours::black);
    g.drawRect(bounds, 1);

    // text in progress bar
    g.setFont(16.0f);
    g.drawFittedText(String(duration * elapsedTime, 2) + " s",
                     bounds,
                     Justification::centred, 1);

    // progress bar
    bounds.reduce(3,3);
    bounds.setWidth((int)(bounds.getWidth() * elapsedTime));
    g.setColour(Colours::blue.withAlpha(0.4f));
    g.fillRect(bounds);
}

void Polytempo_Progressbar::setText(String text_) { text.reset(new String(text_)); }
void Polytempo_Progressbar::setTime(int time_) { time = time_; }
void Polytempo_Progressbar::setDuration(float duration_) { duration = duration_; }

void Polytempo_Progressbar::eventNotification(Polytempo_Event* event)
{
    // update progress bar on every tick
    if (event->getType() == eventType_Tick)
    {
        if ((float)event->getValue() == time * 0.001f) elapsedTime = 0;
        else elapsedTime = ((float)event->getValue() - time * 0.001f) / (float)duration;
        MessageManager::callAsync([this]() { repaint(); });
    }
    // update progress bar when jumping to a point in time
    else if (event->getType() == eventType_GotoTime)
    {
        if ((float)event->getValue() == time * 0.001f) elapsedTime = 0;
        else elapsedTime = ((float)event->getValue() - time * 0.001f) / (float)duration;
        MessageManager::callAsync([this]() { repaint(); });
    }

    // remove when time is over
    if (elapsedTime >= 1)
    {
        // call on the message thread
        MessageManager::callAsync([this]() {
            auto parent = (Polytempo_GraphicsViewRegion*)getParentComponent();
            if (parent != nullptr) parent->clear();
        });
    }
}
