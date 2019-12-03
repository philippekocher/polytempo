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
    ~Polytempo_NetworkMainView();

    void paint(Graphics& g) override;
    void resized() override;
    void changeListenerCallback(ChangeBroadcaster* source) override;

private:
    Polytempo_VisualMetro* visualMetro;
    Polytempo_GraphicsView* graphicsView;
    Polytempo_AuxiliaryView* auxiliaryView;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_NetworkMainView)
};
