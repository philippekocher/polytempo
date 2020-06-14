#include "JuceHeader.h"
#include "Polytempo_TempoMapComponent.h"
#include "../../Misc/Polytempo_Globals.h"
#include "../../Preferences/Polytempo_StoredPreferences.h"


Polytempo_TempoMapComponent::Polytempo_TempoMapComponent()
{
    coordinateSystem.reset(new Polytempo_CoordinateSystem());
    addAndMakeVisible(coordinateSystem.get());
    coordinateSystem->setSynchronizedViewport(&timeRuler,0);
    coordinateSystem->setSynchronizedViewport(&tempoRuler,1);
    
    tempoMapCoordinateSystem.reset(new Polytempo_TempoMapCoordinateSystem(coordinateSystem.get()));
    coordinateSystem->setViewedComponent(tempoMapCoordinateSystem.get(), false);
    
    addAndMakeVisible(timeRuler);
    addAndMakeVisible(tempoRuler);
    
    timeRuler.setSynchronizedViewport(coordinateSystem.get(), 0);
    tempoRuler.setSynchronizedViewport(coordinateSystem.get(), 1);
    
    coordinateSystem->setViewPositionProportionately(0.0, 1.0);

    Polytempo_StoredPreferences::getInstance()->getProps().addChangeListener(this);
    zoomX = float(Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("zoomX"));
    zoomY = float(Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("tempoMapZoomY"));
}

Polytempo_TempoMapComponent::~Polytempo_TempoMapComponent()
{
    tempoMapCoordinateSystem = nullptr;
    coordinateSystem = nullptr;
}

void Polytempo_TempoMapComponent::paint (Graphics& g)
{
    g.fillAll(Colour (245,245,245));
    
    g.setColour(Colours::grey);
    g.drawVerticalLine(getWidth() - 1, 0.0f, float(getHeight()));
}

void Polytempo_TempoMapComponent::resized()
{
    Rectangle<int> r (getLocalBounds());
    
    coordinateSystem->setBounds(r.withTrimmedBottom(40).withTrimmedLeft(70).withTrimmedRight(1));
    timeRuler.setBounds(r.removeFromBottom(40).withTrimmedLeft(70).withTrimmedRight(1));
    tempoRuler.setBounds(r.removeFromLeft(70).withTrimmedLeft(5));

    // resize content while retaining scroll position
    int height = tempoMapCoordinateSystem->getHeight();
    float x = (coordinateSystem->getViewPositionX() - TIMEMAP_OFFSET) / zoomX;
    float y = (coordinateSystem->getViewPositionY() + TIMEMAP_OFFSET + coordinateSystem->getMaximumVisibleHeight()) / zoomY;

    zoomX = float(Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("zoomX"));
    zoomY = float(Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("tempoMapZoomY"));

    int width = int(relativeWidth * zoomX);
    if(width < coordinateSystem->getMaximumVisibleWidth())
        width = coordinateSystem->getMaximumVisibleWidth();

    height = int(zoomY);
    if(height < coordinateSystem->getMaximumVisibleHeight())
        height = coordinateSystem->getMaximumVisibleHeight();
    
    tempoMapCoordinateSystem->setSizeAndZooms(width, height, zoomX, zoomY);
    tempoRuler.setSizeAndZooms(0, height, zoomX, zoomY);
    timeRuler.setSizeAndZooms(width, 0, zoomX, zoomY);

    coordinateSystem->setViewPosition(TIMEMAP_OFFSET + int(x*zoomX), int(y*zoomY) - TIMEMAP_OFFSET - coordinateSystem->getMaximumVisibleHeight());
}

void Polytempo_TempoMapComponent::setRelativeSize(float maxTime)
{
    relativeWidth = maxTime;
    
    resized();
}

void Polytempo_TempoMapComponent::changeListenerCallback (ChangeBroadcaster*)
{
    resized();
    repaint();
}
