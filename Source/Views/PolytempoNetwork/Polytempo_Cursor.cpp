#include "Polytempo_Cursor.h"
#include "Polytempo_GraphicsViewRegion.h"

Polytempo_Cursor::Polytempo_Cursor()
{
    setOpaque(false);
    elapsedTime = 0;
    time = 0;
}

Polytempo_Cursor::~Polytempo_Cursor()
{
}

void Polytempo_Cursor::paint(Graphics& g)
{
    Rectangle<int> bounds = getLocalBounds();
    g.setColour(Colours::blue.withAlpha(0.4f));
    g.drawVerticalLine((int)(bounds.getWidth() * elapsedTime), 0.0f, (float)getHeight());
}

void Polytempo_Cursor::setTime(int time_) { time = time_; }
void Polytempo_Cursor::setX(float x) { elapsedTime = x; }
void Polytempo_Cursor::setIncrement(float incr) { increment = incr; }

void Polytempo_Cursor::eventNotification(Polytempo_Event* event)
{
    // update progress bar on every tick
    if (event->getType() == eventType_Tick)
    {
        float tick = (float)event->getValue();

        if (tick <= time * 0.001f) elapsedTime = 0;
        else elapsedTime = (tick - time * 0.001f) * (float)increment;
        MessageManager::callAsync([this]() { repaint(); });
    }
    // update progress bar when jumping to a point in time
    else if (event->getType() == eventType_GotoTime)
    {
        if (approximatelyEqual((float)event->getValue(), time * 0.001f)) elapsedTime = 0;
        else elapsedTime = ((float)event->getValue() - time * 0.001f) * (float)increment;
        MessageManager::callAsync([this]() { repaint(); });
    }
}
