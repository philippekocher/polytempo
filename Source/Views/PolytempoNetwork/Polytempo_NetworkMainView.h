#pragma once

#include "JuceHeader.h"

class Polytempo_VisualMetro;
class Polytempo_GraphicsView;
class Polytempo_AuxiliaryView;

//------------------------------------------------------------------------------
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class Polytempo_NetworkMainView : public Component,
                                  public ChangeListener
{
public:
    Polytempo_NetworkMainView();
    ~Polytempo_NetworkMainView() override;

    void paint(Graphics& g) override;
    void resized() override;
    void changeListenerCallback(ChangeBroadcaster* source) override;

    class BlackPanel : public Component
    {
    public:
        void paint(Graphics &g)
        {
            g.fillAll(Colours::black.withAlpha(alpha));
        }
        float alpha = 0;
    };
    BlackPanel* blackPanel;
    void setBrightness(float amount) { if (amount > 1.0f) amount = 1.0f; if (amount < 0) amount = 0; blackPanel->alpha = 1.0f-amount; repaint(); }

private:
    Polytempo_VisualMetro* visualMetro;
    Polytempo_GraphicsView* graphicsView;
    Polytempo_AuxiliaryView* auxiliaryView;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_NetworkMainView)
};
