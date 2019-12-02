#include "Polytempo_Progressbar.h"
#include "Polytempo_GraphicsViewRegion.h"

Polytempo_Progressbar::Polytempo_Progressbar()
{
    setOpaque(false);
    
    elapsedTime = 0.0;
}

Polytempo_Progressbar::~Polytempo_Progressbar()
{}

void Polytempo_Progressbar::paint(Graphics &g)
{
    Rectangle<int>bounds = getLocalBounds();
    
    // text above
    bounds.setHeight(bounds.getHeight() / 2);
    g.setFont(24.0f);
    g.setColour(Colours::black);
    g.drawFittedText(*text,
                     bounds,  // inset rect
                     Justification::topLeft, 1);
    
    // text in progress bar
    bounds.setY(bounds.getHeight());
    g.setFont(16.0f);
    g.drawFittedText(String(duration * elapsedTime,2) + " s",
                     bounds.reduced(2,2),  // inset rect
                     Justification::centred, 1);

    // progress bar
    g.setColour(Colours::grey);
    g.drawRect(bounds);
    
    bounds.setWidth((int)(bounds.getWidth() * elapsedTime));
    g.setColour(Colours::blue.withAlpha(0.2f));
    g.fillRect(bounds);
}

void Polytempo_Progressbar::setText(String text_)      { text.reset(new String(text_)); }
void Polytempo_Progressbar::setTime(int time_)         { time = time_; }
void Polytempo_Progressbar::setDuration(float duration_) { duration = duration_; }


void Polytempo_Progressbar::eventNotification(Polytempo_Event *event)
{
    // update progress bar on every tick
    if(event->getType() == eventType_Tick)
    {
        if((float)event->getValue() == time * 0.001f) elapsedTime = 0;
        else  elapsedTime = ((float)event->getValue() - time * 0.001f) / (float)duration;
        repaint();
    }
    // update progress bar when jumping to a point in time
    else if(event->getType() == eventType_GotoTime)
    {
        if((float)event->getValue() == time * 0.001f) elapsedTime = 0;
        else  elapsedTime = ((float)event->getValue() - time * 0.001f) / (float)duration;
        repaint();
    }
    
    // remove when time is over
    if(elapsedTime >= 1)
    {
        ((Polytempo_GraphicsViewRegion*)getParentComponent())->clear();
    }
}
