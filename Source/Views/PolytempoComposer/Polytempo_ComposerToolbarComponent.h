#pragma once

#include "JuceHeader.h"
#include "../../Misc/Polytempo_Button.h"


class Polytempo_ComposerToolbarComponent : public Component,
                                           public Button::Listener
{
public:
    Polytempo_ComposerToolbarComponent();
    ~Polytempo_ComposerToolbarComponent();

    void paint (Graphics&);
    void resized();

    void buttonClicked(Button*);
    
    void setComponentTypes(int left, int right);

private:
    std::unique_ptr<Polytempo_Button> showTimeMapButton;
    std::unique_ptr<Polytempo_Button> showTempoMapButton;
    std::unique_ptr<Polytempo_Button> showBeatPatternListButton;
    std::unique_ptr<Polytempo_Button> showPointEditorButton;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_ComposerToolbarComponent)
};
