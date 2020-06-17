#include "Polytempo_NetworkMainView.h"
#include "Polytempo_VisualMetro.h"
#include "Polytempo_GraphicsView.h"
#include "Polytempo_AuxiliaryView.h"
#include "../../Preferences/Polytempo_StoredPreferences.h"

Polytempo_NetworkMainView::Polytempo_NetworkMainView()
{
    Polytempo_StoredPreferences::getInstance()->getProps().addChangeListener(this);

    addAndMakeVisible(visualMetro = new Polytempo_VisualMetro());
    addAndMakeVisible(graphicsView = new Polytempo_GraphicsView());
    addAndMakeVisible(auxiliaryView = new Polytempo_AuxiliaryView());

    addAndMakeVisible(blackPanel = new BlackPanel);
    blackPanel->setAlwaysOnTop(true);
}

Polytempo_NetworkMainView::~Polytempo_NetworkMainView()
{
    deleteAllChildren();
}

void Polytempo_NetworkMainView::paint(Graphics& g)
{
    g.fillAll(Colours::white);
}

void Polytempo_NetworkMainView::resized()
{
    int stripWidth, stripLength, auxiliaryViewHeight, auxiliaryViewWidth;

    if (!Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("showAuxiliaryView"))
        auxiliaryViewHeight = auxiliaryViewWidth = 0;
    else
    {
        auxiliaryViewWidth = (int)(getWidth() * 0.12);
        auxiliaryViewWidth = auxiliaryViewWidth < 120 ? 120 : auxiliaryViewWidth > 200 ? 200 : auxiliaryViewWidth;

        auxiliaryViewHeight = 0;
    }

    auxiliaryView->setBounds(getWidth() - auxiliaryViewWidth,
                             0,
                             auxiliaryViewWidth,
                             getHeight());

    if (!Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("showVisualMetro"))
        stripWidth = 0;
    else
    {
        stripWidth = Polytempo_StoredPreferences::getInstance()->getProps().getIntValue("stripWidth");

        stripLength = getWidth() - auxiliaryViewWidth < getHeight() - auxiliaryViewHeight ? getWidth() - auxiliaryViewWidth : getHeight() - auxiliaryViewHeight;

        visualMetro->setBounds(0, 0, stripLength, stripLength);
        visualMetro->setWidth((float)stripWidth);
    }

    graphicsView->setBounds(stripWidth,
                            stripWidth,
                            getWidth() - stripWidth - auxiliaryViewWidth,
                            getHeight() - stripWidth - auxiliaryViewHeight);
    
    blackPanel->setBounds(getBounds());
}

void Polytempo_NetworkMainView::changeListenerCallback(ChangeBroadcaster*)
{
    // settings have changed

    Colour fgColour = Colour::fromString(Polytempo_StoredPreferences::getInstance()->getProps().getValue("stripNormalColour"));
    Colour bgColour = Colour::fromString(Polytempo_StoredPreferences::getInstance()->getProps().getValue("stripBackgroundColour"));
    Colour frColour = Colour::fromString(Polytempo_StoredPreferences::getInstance()->getProps().getValue("stripFrameColour"));

    visualMetro->setForegroundColour(fgColour);
    visualMetro->setBackgroundColour(bgColour);
    visualMetro->setFrameColour(frColour);

    resized();
    repaint();
}
