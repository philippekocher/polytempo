#pragma once

#include "../../Application/PolytempoComposer/Polytempo_ComposerApplication.h"
#include "../../Misc/Polytempo_TempoMeasurement.h"
#include "../../Misc/Polytempo_ImageButton.h"

namespace Polytempo_DialogWindows
{
    class Basic : public DocumentWindow,
                  public Button::Listener,
                  public Label::Listener,
                  public TextEditor::Listener
    {
    public:
        Basic() : DocumentWindow("Basic", Colours::white, 0)
        {
            contentComponent = new Component();

            cancelButton = new TextButton("Cancel");
            cancelButton->addListener(this);
            cancelButton->setWantsKeyboardFocus(false);
            contentComponent->addAndMakeVisible(cancelButton);

            setContentOwned(contentComponent, false);
            setUsingNativeTitleBar(true);
            setSize(360, 100);
        }
        
        ~Basic()
        {
            contentComponent->deleteAllChildren();
        }
        
        void setSize(int w, int h)
        {
            width = w;
            height = h;

            Polytempo_ComposerApplication* const app = dynamic_cast<Polytempo_ComposerApplication*>(JUCEApplication::getInstance());
            centreAroundComponent(&app->getMainView(), width, height);

            Rectangle<int> rect = getLocalBounds();
            contentComponent->setBounds(rect);

            cancelButton->setBounds(width-80, height-30, 60, 20);
        }
        
        void show()
        {
            setWantsKeyboardFocus(false);
            runModalLoop();
        }
        
        void addOkButton(const String buttonText)
        {
            okButton = new TextButton(buttonText);
            okButton->addListener(this);
            okButton->setWantsKeyboardFocus(false);
            contentComponent->addAndMakeVisible(okButton);

            okButton->setBounds(width-80, height-30, 60, 20);
            cancelButton->setBounds(width-150, height-30, 60, 20);
       }
        
        virtual void perform(Button *button) = 0;
        
        void buttonClicked(Button *button)
        {
            if(button == cancelButton)
            {
                setVisible(false);
            }
            else
            {
                perform(button);
                setVisible(false);
            }
        }

        bool keyPressed(const KeyPress& kp)
        {
            if(kp == KeyPress::escapeKey)
            {
                setVisible(false);
                return true;
            }
            if(kp == KeyPress::returnKey && okButton->isEnabled())
            {
                perform(okButton);
                setVisible(false);
                return true;
            }
            
            return false;
        }
        
    protected:
        int height, width;
        Component *contentComponent;
        TextButton *okButton = nullptr;
        TextButton *cancelButton;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Basic)
    };
    
    
    class InsertControlPoint : public Basic
    {
    public:
        InsertControlPoint()
        {
            setName("Insert Control Point");
            addOkButton("Add");
            okButton->setEnabled(false);
            
            contentComponent->addAndMakeVisible(timeTextbox = new Polytempo_Textbox("Time [s]"));
            timeTextbox->setBounds(20,25,80,18);
            timeTextbox->setText("0", dontSendNotification);
            timeTextbox->setInputRestrictions(0,"0123456789.");
            timeTextbox->addListener(this);
            
            contentComponent->addAndMakeVisible(positionTextbox = new Polytempo_Textbox("Position"));
            positionTextbox->setBounds(120,25,80,18);
            positionTextbox->setText("0", dontSendNotification);
            positionTextbox->setInputRestrictions(0,"0123456789/");
            positionTextbox->addListener(this);
            
            contentComponent->addAndMakeVisible(tempoInTextbox = new Polytempo_Textbox("Tempo In"));
            tempoInTextbox->setBounds(220,25,50,18);
            tempoInTextbox->setText(String(Polytempo_TempoMeasurement::decodeTempoForUI(0.25)), dontSendNotification);
            tempoInTextbox->addListener(this);

            contentComponent->addAndMakeVisible(tempoOutTextbox = new Polytempo_Textbox("Tempo Out"));
            tempoOutTextbox->setBounds(290,25,50,18);
            tempoOutTextbox->setText(String(Polytempo_TempoMeasurement::decodeTempoForUI(0.25)), dontSendNotification);
            tempoOutTextbox->addListener(this);
            
            validate();
        }
        
        void validate()
        {
            Polytempo_Sequence* sequence = Polytempo_Composition::getInstance()->getSelectedSequence();
            okButton->setEnabled(sequence->validateNewControlPointPosition(timeTextbox->getText().getFloatValue(), Rational(positionTextbox->getText())));
        }
        
        void textEditorTextChanged(TextEditor &)
        {
            validate();
        }
        
        void labelTextChanged (Label* /*label*/)
        {
            validate();
        }
 
        void perform(Button *button)
        {
            if(button == okButton)
            {
                Polytempo_Sequence* sequence = Polytempo_Composition::getInstance()->getSelectedSequence();
            
                sequence->addControlPoint(timeTextbox->getText().getFloatValue(),
                                          Rational(positionTextbox->getText()),
                                          Polytempo_TempoMeasurement::encodeTempoFromUI((tempoInTextbox->getText().getFloatValue())),
                                          Polytempo_TempoMeasurement::encodeTempoFromUI(tempoOutTextbox->getText().getFloatValue()));
            }
        }
        
    private:
        Polytempo_Textbox *timeTextbox;
        Polytempo_Textbox *positionTextbox;
        Polytempo_Textbox *tempoInTextbox;
        Polytempo_Textbox *tempoOutTextbox;
    };
    
    
    class ShiftControlPoints : public Basic
    {
    public:
        ShiftControlPoints()
        {
            setSize(360, 190);

            setName("Shift Selected Control Points");
            addOkButton("Ok");

            Polytempo_Composition* composition = Polytempo_Composition::getInstance();
            Array<int> *indices = composition->getSelectedControlPointIndices();
            firstTime = composition->getSelectedSequence()->getControlPoints()->getUnchecked(indices->getFirst())->time;
            lastTime  = composition->getSelectedSequence()->getControlPoints()->getUnchecked(indices->getLast())->time;
            deltaTime = 0;
            firstPosition = composition->getSelectedSequence()->getControlPoints()->getUnchecked(indices->getFirst())->position;
            lastPosition  = composition->getSelectedSequence()->getControlPoints()->getUnchecked(indices->getLast())->position;
            deltaPosition = 0;

            contentComponent->addAndMakeVisible(shiftInTimeLabel = new Label("ShiftInTimeLabel","Shift in Time"));
            shiftInTimeLabel->setBounds(16, 15, 200, 20);
            shiftInTimeLabel->setFont(Font(16));
            shiftInTimeLabel->setMinimumHorizontalScale(1.0f);

            contentComponent->addAndMakeVisible(deltaTimeTextbox = new Polytempo_Textbox("shift points by"));
            deltaTimeTextbox->setBounds(20, 50, 90, 18);
            deltaTimeTextbox->setText(String(deltaTime), dontSendNotification);
            deltaTimeTextbox->setInputRestrictions(0,"0123456789.-");
            deltaTimeTextbox->addListener(this);
            
            contentComponent->addAndMakeVisible(firstTimeTextbox = new Polytempo_Textbox("align first point with"));
            firstTimeTextbox->setBounds(135, 50, 90, 18);
            firstTimeTextbox->setText(String(firstTime), dontSendNotification);
            firstTimeTextbox->setInputRestrictions(0,"0123456789.-");
            firstTimeTextbox->addListener(this);

            contentComponent->addAndMakeVisible(lastTimeTextbox = new Polytempo_Textbox("align last point with"));
            lastTimeTextbox->setBounds(250, 50, 90, 18);
            lastTimeTextbox->setText(String(lastTime), dontSendNotification);
            lastTimeTextbox->setInputRestrictions(0,"0123456789.-");
            lastTimeTextbox->addListener(this);

            contentComponent->addAndMakeVisible(shiftInPositionLabel = new Label("ShiftInPositionLabel","Shift in Position"));
            shiftInPositionLabel->setBounds(16, 85, 200, 20);
            shiftInPositionLabel->setFont(Font(16));
            shiftInPositionLabel->setMinimumHorizontalScale(1.0f);

            contentComponent->addAndMakeVisible(deltaPositionTextbox = new Polytempo_Textbox("shift points by"));
            deltaPositionTextbox->setBounds(20, 120, 90, 18);
            deltaPositionTextbox->setText(deltaPosition.toString(), dontSendNotification);
            deltaPositionTextbox->setInputRestrictions(0,"0123456789-/");
            deltaPositionTextbox->addListener(this);
            
            contentComponent->addAndMakeVisible(firstPositionTextbox = new Polytempo_Textbox("align first point with"));
            firstPositionTextbox->setBounds(135, 120, 90, 18);
            firstPositionTextbox->setText(firstPosition.toString(), dontSendNotification);
            firstPositionTextbox->setInputRestrictions(0,"0123456789-/");
            firstPositionTextbox->addListener(this);

            contentComponent->addAndMakeVisible(lastPositionTextbox = new Polytempo_Textbox("align last point with"));
            lastPositionTextbox->setBounds(250, 120, 90, 18);
            lastPositionTextbox->setText(lastPosition.toString(), dontSendNotification);
            lastPositionTextbox->setInputRestrictions(0,"0123456789-/");
            lastPositionTextbox->addListener(this);
        }
        
        void labelTextChanged (Label* label)
        {
            if(label == deltaTimeTextbox)      deltaTime = label->getText().getFloatValue();
            else if(label == firstTimeTextbox) deltaTime = label->getText().getFloatValue() - firstTime;
            else if(label == lastTimeTextbox)  deltaTime = label->getText().getFloatValue() - lastTime;

            else if(label == deltaPositionTextbox) deltaPosition = Rational(label->getText());
            else if(label == firstPositionTextbox) deltaPosition = Rational(label->getText()) - firstPosition;
            else if(label == lastPositionTextbox)  deltaPosition = Rational(label->getText()) - lastPosition;

            deltaTimeTextbox->setText(String(deltaTime), dontSendNotification);
            firstTimeTextbox->setText(String(firstTime + deltaTime), dontSendNotification);
            lastTimeTextbox->setText(String(lastTime + deltaTime), dontSendNotification);
            
            deltaPositionTextbox->setText(deltaPosition.toString(), dontSendNotification);
            firstPositionTextbox->setText((firstPosition+deltaPosition).toString(), dontSendNotification);
            lastPositionTextbox->setText((lastPosition+deltaPosition).toString(), dontSendNotification);
        }
 
        void perform(Button *button)
        {
            if(button == okButton)
            {
                Polytempo_Composition* composition = Polytempo_Composition::getInstance();
                Polytempo_Sequence* sequence = composition->getSelectedSequence();
            
                sequence->shiftControlPoints(composition->getSelectedControlPointIndices(), deltaTime, deltaPosition);
            }
        }
        
    private:
        Label *shiftInTimeLabel;
        Label *shiftInPositionLabel;

        Polytempo_Textbox *deltaTimeTextbox;
        Polytempo_Textbox *firstTimeTextbox;
        Polytempo_Textbox *lastTimeTextbox;

        Polytempo_Textbox *deltaPositionTextbox;
        Polytempo_Textbox *firstPositionTextbox;
        Polytempo_Textbox *lastPositionTextbox;

        float firstTime;
        float lastTime;
        float deltaTime;
        
        Rational firstPosition;
        Rational lastPosition;
        Rational deltaPosition;
    };
    
    class AdjustControlPoints : public Basic
    {
       public:
        AdjustControlPoints()
       {
           setSize(200, 330);
           setName("Adjust Selected Control Points");
           
           contentComponent->addAndMakeVisible(adjustTimeFwdButton = new Polytempo_ImageButton());
           adjustTimeFwdButton->setBounds(20, 20, 50, 50);
           adjustTimeFwdButton->setImages(Drawable::createFromImageData(BinaryData::adjustTimeFwd_png, BinaryData::adjustTimeFwd_pngSize).get());
           adjustTimeFwdButton->addListener(this);
           
           contentComponent->addAndMakeVisible(adjustTimeFwdLabel = new Label("AdjustTimeFwdLabel","Adjust time relative to previous control point"));
           adjustTimeFwdLabel->setBounds(90, 20, 100, 50);
           adjustTimeFwdLabel->setFont(Font(12));
           adjustTimeFwdLabel->setMinimumHorizontalScale(1.0f);
           
           contentComponent->addAndMakeVisible(adjustTimeBwdButton = new Polytempo_ImageButton());
           adjustTimeBwdButton->setBounds(20, 90, 50, 50);
           adjustTimeBwdButton->setImages(Drawable::createFromImageData(BinaryData::adjustTimeBwd_png, BinaryData::adjustTimeBwd_pngSize).get());
           adjustTimeBwdButton->addListener(this);
           
           contentComponent->addAndMakeVisible(adjustTimeBwdLabel = new Label("AdjustTimeBwdLabel", "Adjust time relative to next control point"));
           adjustTimeBwdLabel->setBounds(90, 90, 100, 50);
           adjustTimeBwdLabel->setFont(Font(12));
           adjustTimeBwdLabel->setMinimumHorizontalScale(1.0f);

           contentComponent->addAndMakeVisible(adjustPositionFwdButton = new Polytempo_ImageButton());
           adjustPositionFwdButton->setBounds(20, 160, 50, 50);
           adjustPositionFwdButton->setImages(Drawable::createFromImageData(BinaryData::adjustPositionFwd_png, BinaryData::adjustPositionFwd_pngSize).get());
           adjustPositionFwdButton->addListener(this);
           
           contentComponent->addAndMakeVisible(adjustPositionFwdLabel = new Label("AdjustPositionFwdLabel","Adjust position relative to previous control point"));
           adjustPositionFwdLabel->setBounds(90, 160, 100, 50);
           adjustPositionFwdLabel->setFont(Font(12));
           adjustPositionFwdLabel->setMinimumHorizontalScale(1.0f);
           
           contentComponent->addAndMakeVisible(adjustPositionBwdButton = new Polytempo_ImageButton());
           adjustPositionBwdButton->setBounds(20, 230, 50, 50);
           adjustPositionBwdButton->setImages(Drawable::createFromImageData(BinaryData::adjustPositionBwd_png, BinaryData::adjustPositionBwd_pngSize).get());
           adjustPositionBwdButton->addListener(this);

           contentComponent->addAndMakeVisible(adjustPositionBwdLabel = new Label("AdjustPositionBwdLabel","Adjust position relative to next control point"));
           adjustPositionBwdLabel->setBounds(90, 230, 100, 50);
           adjustPositionBwdLabel->setFont(Font(12));
           adjustPositionBwdLabel->setMinimumHorizontalScale(1.0f);
           
       }
    
       void labelTextChanged (Label* label)
       {}

       void perform(Button *button)
       {
           Polytempo_Composition* composition = Polytempo_Composition::getInstance();
           Polytempo_Sequence* sequence = composition->getSelectedSequence();

           if(button == adjustTimeFwdButton)          sequence->adjustTime(composition->getSelectedControlPointIndices(), true);
           else if(button == adjustTimeBwdButton)     sequence->adjustTime(composition->getSelectedControlPointIndices(), false);
           else if(button == adjustPositionFwdButton) sequence->adjustPosition(composition->getSelectedControlPointIndices(), true);
           else if(button == adjustPositionBwdButton) sequence->adjustPosition(composition->getSelectedControlPointIndices(), false);
       }
    private:
        Polytempo_ImageButton *adjustTimeFwdButton;
        Polytempo_ImageButton *adjustTimeBwdButton;
        Polytempo_ImageButton *adjustPositionFwdButton;
        Polytempo_ImageButton *adjustPositionBwdButton;
        
        Label *adjustTimeFwdLabel;
        Label *adjustTimeBwdLabel;
        Label *adjustPositionFwdLabel;
        Label *adjustPositionBwdLabel;
    };
       
    class ExportSequences : public Basic
    {
    public:
        ExportSequences(int num)
        {
            setSize(360, 200);

            if(num == 1) setName("Export Selected Sequence");
            else         setName("Export All Sequences");
            addOkButton("Export");
            
            tbPlain = new ToggleButton(" 1 2 3 ... - Plain (.txt)");
            tbPlain->setRadioGroupId(1234);
            tbPlain->setBounds(20, 20, getWidth() - 40, 22);
            tbPlain->setToggleState (true, dontSendNotification);
            tbPlain->addListener(this);
            contentComponent->addAndMakeVisible(tbPlain);

            tbLisp = new ToggleButton(" (1 2 3 ...) - Lisp Style (.txt)");
            tbLisp->setRadioGroupId(1234);
            tbLisp->setBounds(20, 20 + 30, getWidth() - 40, 22);
            contentComponent->addAndMakeVisible(tbLisp);
            
            tbC = new ToggleButton(" [1, 2, 3 ...] - C Style (.txt)");
            tbC->setRadioGroupId(1234);
            tbC->setBounds(20, 20 + 60, getWidth() - 40, 22);
            contentComponent->addAndMakeVisible(tbC);
            
            tbPolytempo = new ToggleButton(" Polytempo Score (.ptsco)");
            tbPolytempo->setRadioGroupId(1234);
            tbPolytempo->setBounds(20, 20 + 100, getWidth() - 40, 22);
            contentComponent->addAndMakeVisible(tbPolytempo);
        }
        
        void textEditorTextChanged(TextEditor &)
        {}
        
        void labelTextChanged (Label*)
        {}
        
        void perform(Button *button)
        {
            if(button == okButton)
            {    Polytempo_Composition *composition = Polytempo_Composition::getInstance();
            
                if     (tbPlain->getToggleState() == true)     composition->exportAsPlainText();
                else if(tbLisp->getToggleState() == true)      composition->exportAsLispList();
                else if(tbC->getToggleState() == true)         composition->exportAsCArray();
                else if(tbPolytempo->getToggleState() == true) composition->exportAsPolytempoScore();
            }
        }
        
    private:
        ToggleButton *tbPlain, *tbLisp, *tbC, *tbPolytempo;
    };
}
