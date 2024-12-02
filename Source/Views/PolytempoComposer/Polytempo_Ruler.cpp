#include "Polytempo_Ruler.h"
#include "../../Misc/Polytempo_Globals.h"
#include "../../Preferences/Polytempo_StoredPreferences.h"
#include "../../Data/Polytempo_Composition.h"
#include "../../Misc/Polytempo_TempoMeasurement.h"


Polytempo_Ruler::Polytempo_Ruler()
{}

Polytempo_Ruler::~Polytempo_Ruler()
{}

void Polytempo_Ruler::visibleAreaChanged(const Rectangle<int> &r)
{
    if(synchronizedViewport != nullptr)
    {
        if(synchronizedAxis == 0)
            synchronizedViewport->setViewPosition(r.getX(),synchronizedViewport->getViewPositionY());
        else if(synchronizedAxis == 1)
            synchronizedViewport->setViewPosition(synchronizedViewport->getViewPositionX(),r.getY());
        else
            synchronizedViewport->setViewPosition(r.getX(),r.getY());
    }
}

void Polytempo_Ruler::setSynchronizedViewport(Viewport *sync, int axis)
{
    synchronizedViewport = sync;
    synchronizedAxis = axis;
}

void Polytempo_Ruler::setSizeAndZooms(int w, int h, float zX, float zY)
{
    rulerComponent->setSizeAndZooms(w, h, zX, zY);
}

//------------------------------------------------------------------------------
#pragma mark -
#pragma mark ruler component

void Polytempo_RulerComponent::setSizeAndZooms(int w, int h, float zX, float zY)
{
    int width, height;

    if(w > 0) width  = w; else width  = getWidth();
    if(h > 0) height = h; else height = getHeight();
    
    zoomX = zX;
    zoomY = zY;

    setSize(width, height);
}

//------------------------------------------------------------------------------
#pragma mark -
#pragma mark time ruler

Polytempo_TimeRuler::Polytempo_TimeRuler()
{
    rulerComponent.reset(new Polytempo_TimeRulerComponent());

    setScrollBarsShown(false, true);
    setViewedComponent(rulerComponent.get(), false);
    rulerComponent->setBounds(Rectangle<int> (3800, 40));
}

int Polytempo_TimeRuler::getIncrementForZoom(float zoomX)
{
    if(zoomX < 0.4)
    {
        return 300;
    }
    else if(zoomX < 0.8)
    {
        return 120;
    }
    else if(zoomX < 1.6)
    {
        return 60;
    }
    else if(zoomX < 3)
    {
        return 30;
    }
    else if(zoomX < 5)
    {
        return 20;
    }
    else if(zoomX < 10)
    {
        return 10;
    }
    else if(zoomX < 50)
    {
        return 5;
    }
    else
    {
        return 1;
    }
}

Polytempo_TimeRulerComponent::Polytempo_TimeRulerComponent()
{
    zoomX = float(Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("zoomX"));
}

void Polytempo_TimeRulerComponent::paint(Graphics& g)
{
    int hour, min, sec = 0;
    int increment = Polytempo_TimeRuler::getIncrementForZoom(zoomX);
       
    g.setColour(Colour(50,50,50));
    g.setFont(12);
    
    String label = String();
    for(int i=0;i<100;i++)
    {
        min = (sec / 60) % 60;
        hour = (sec / 3600);
        
        if(hour == 0)
        {
            label = "";
            label << String(min).paddedLeft('0',2) << ":" << String(sec%60).paddedLeft('0',2);
        }
        else
        {
            label = "";
            label << String(hour) << ":" << String(min).paddedLeft('0',2) << ":" << String(sec%60).paddedLeft('0',2);
        }
        g.drawVerticalLine(TIMEMAP_OFFSET + int(sec * zoomX), 0.0f, float(getHeight()));
        g.drawText(label, TIMEMAP_OFFSET + 2 + int(sec * zoomX), 1, 50, 12, Justification::left);

        sec += increment;
    }
}

//------------------------------------------------------------------------------
#pragma mark -
#pragma mark position ruler

Polytempo_PositionRuler::Polytempo_PositionRuler()
{
    rulerComponent.reset(new Polytempo_PositionRulerComponent());
    
    setScrollBarsShown(false, false, true, true);
    setViewedComponent(rulerComponent.get(), false);
    rulerComponent->setBounds(Rectangle<int>(65, 3800));
}

Polytempo_PositionRulerComponent::Polytempo_PositionRulerComponent()
{
    zoomY = float(Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("timeMapZoomY"));
}

void Polytempo_PositionRulerComponent::paint(Graphics& g)
{
    Polytempo_Sequence* sequence = Polytempo_Composition::getInstance()->getSelectedSequence();
    if(sequence == nullptr) return;

    g.setColour(sequence->getColour());
    g.fillRect(0,0,10,getHeight());
    
    if(sequence->getSelectedBeatPatternIndex() > -1)
    {
        Polytempo_BeatPattern* bp = sequence->getSelectedBeatPattern();
        if(bp)
        {
            Rational length = bp->getLength() * bp->getRepeats();
            g.setColour(LookAndFeel::getDefaultLookAndFeel().findColour(0x1001600).withAlpha(0.1f));
            g.fillRect(11,
                       int(getHeight() - TIMEMAP_OFFSET - (bp->getStartPosition() + length) * zoomY),
                       getWidth(),
                       int(length * zoomY));
        }
    }

    g.setFont(12);
    g.setColour(Colour(50,50,50));
    Polytempo_Event* event;
    Rational pos, labelPos = 0;
    int labelOffset = 0;
    String label;
    float y, thickness;
    int i = -1;
    while((event = sequence->getEvent(++i)) != nullptr)
    {
        pos = event->getPosition();
        if(event->getType() == eventType_Beat)
        {
            y = getHeight() - TIMEMAP_OFFSET - pos * zoomY;
            if(int(event->getProperty(eventPropertyString_Pattern)) < 20)
            {
                thickness = int(event->getProperty(eventPropertyString_Pattern)) == 10 ? 2.0f : 1.0f;

                g.drawLine(45, y, float(getWidth()), y, thickness);
            }
            else
            {
                thickness = 0.5;
                g.drawLine(50, y, float(getWidth()), y, thickness);
            }
        }
        else if(event->getType() == eventType_Marker)
        {
            if(pos != labelPos)
            {
                labelPos = pos;
                labelOffset = 0;
            }
            label = event->getProperty(eventPropertyString_Value);
            g.drawFittedText(label, 18+labelOffset, getHeight() - TIMEMAP_OFFSET - 6 - int(pos * zoomY), 10, 10, Justification::left, 1, 0.1f);
            
            labelOffset += 12;
        }
    }
}

//------------------------------------------------------------------------------
#pragma mark -
#pragma mark tempo ruler

Polytempo_TempoRuler::Polytempo_TempoRuler()
{
    rulerComponent.reset(new Polytempo_TempoRulerComponent());

    setScrollBarsShown(false, false, true, true);
    setViewedComponent(rulerComponent.get(), false);
    rulerComponent->setBounds(Rectangle<int>(65, 10000));
}

float Polytempo_TempoRuler::getIncrementForZoom(float zoom)
{
    if(zoom > 8000) return 0.25f / 120; // steps of 0.5
    if(zoom > 4000) return 0.25f / 60;  // steps of 1
    if(zoom > 2000) return 0.25f / 30;  // steps of 2
    if(zoom > 1000) return 0.25f / 12;  // steps of 5
    return 0.25f / 6;                   // steps of 10
}

Polytempo_TempoRulerComponent::Polytempo_TempoRulerComponent()
{
    zoomY = float(Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("tempoMapZoomY"));
}

void Polytempo_TempoRulerComponent::paint(Graphics& g)
{
    Polytempo_Sequence* sequence = Polytempo_Composition::getInstance()->getSelectedSequence();
    if(sequence == nullptr) return;
    
    g.setColour(sequence->getColour());
    g.fillRect(0,0,10,getHeight());
    
    g.setFont(12);
    g.setColour(Colour(50,50,50));
    
    float increment = Polytempo_TempoRuler::getIncrementForZoom(zoomY);
    float tempo = 0;
    int i = 0;
    
    while(tempo < 5.0f)
    {
        float y = getHeight() - TIMEMAP_OFFSET - tempo * zoomY;
        
        if(i++ % 2 == 0)
        {
            g.drawLine(55.0f, y, float(getWidth()), y, 0.5f);
            float num = Polytempo_TempoMeasurement::decodeTempoForUI(tempo);
            int decimalPlaces = num < 10 ? 4 : num < 100 ? 3 : num < 1000 ? 2 : 1;
            g.drawFittedText(String(num, decimalPlaces), 18, int(y - 6), 30, 10, Justification::left, 1);
        }
        else
        {
            g.drawLine(60.0f, y, float(getWidth()), y, 0.5f);
        }
        
        tempo += increment;
    }
}

