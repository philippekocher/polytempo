#pragma once

#include "../../Scheduler/Polytempo_EventObserver.h"
#include "../../Misc/Polytempo_Textbox.h"
#include "../../Misc/Polytempo_Button.h"
#include "Polytempo_TimeSyncControl.h"
#include "Polytempo_NetworkInfoView.h"
#include "Polytempo_AnnotationView.h"

class Polytempo_AuxiliaryView : public Component,
                                public Label::Listener,
                                public Button::Listener,
                                public Polytempo_EventObserver
{
public:
    Polytempo_AuxiliaryView();
    ~Polytempo_AuxiliaryView() override;

    void paint(Graphics&) override;
    void resized() override;
    void eventNotification(Polytempo_Event* event) override;

    /** Called when a Label goes into editing mode */
    void editorShown(Label* label, TextEditor&) override;
    void labelTextChanged(Label* labelThatHasChanged) override;

    void buttonClicked(Button* button) override;
    void buttonStateChanged(Button* button) override;

private:
    Polytempo_Textbox* markerTextbox;
    String markerString;
    Polytempo_Button* markerBackwards;
    Polytempo_Button* markerForwards;
    Polytempo_Button* imageBackwards;
    Polytempo_Button* imageForwards;

    Polytempo_Button* startStop;
    Polytempo_Button* returnToLocator;
    Polytempo_Button* returnToBeginning;

    Polytempo_Textbox* timeTextbox;
    float timeTextboxValue;

    Polytempo_Textbox* tempoFactorTextbox;

    Polytempo_AnnotationView* annotationView;

    Polytempo_TimeSyncControl* timeSyncControl;

    String tempoFactor;

    AttributedString peers;
    Polytempo_NetworkInfoView* networkInfoView;

    int separator1Position;
    int separator2Position;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_AuxiliaryView)
};
