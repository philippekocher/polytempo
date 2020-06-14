#pragma once

#include "JuceHeader.h"
#include "../../Misc/Polytempo_Button.h"
#include "../../Misc/Polytempo_Textbox.h"
#include "../../Scheduler/Polytempo_EventObserver.h"


class Polytempo_TransportComponent : public Component,
                                     public Button::Listener,
                                     public Label::Listener,
                                     public Polytempo_EventObserver
{
public:
    Polytempo_TransportComponent();
    ~Polytempo_TransportComponent();
    
    void paint (Graphics&);
    void resized();
    
    void buttonClicked(Button*);
    void editorShown(Label*, TextEditor&);
    void labelTextChanged(Label*);
    
    void eventNotification(Polytempo_Event*);
    
private:
    std::unique_ptr<Polytempo_Button> startButton;
    std::unique_ptr<Polytempo_Button> returnToLocatorButton;
    std::unique_ptr<Polytempo_Button> returnToZeroButton;
    
    Polytempo_Textbox *timeTextbox;
    String timeString;
    
    Polytempo_Textbox *tempoFactorTextbox;
    String tempoFactorString;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_TransportComponent)
};
