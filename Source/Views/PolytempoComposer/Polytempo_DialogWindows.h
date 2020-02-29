/* ==============================================================================
 
 This file is part of the POLYTEMPO software package.
 Copyright (c) 2016 - Zurich University of the Arts,
 ICST Institute for Computer Music and Sound Technology
 http://www.icst.net
 
 Author: Philippe Kocher
 
 POLYTEMPO is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 POLYTEMPO is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this software. If not, see <http://www.gnu.org/licenses/>.
 
 ============================================================================== */

#ifndef __Polytempo_DialogWindows__
#define __Polytempo_DialogWindows__

#include "../../Application/PolytempoComposer/Polytempo_ComposerApplication.h"
#include "../../Misc/Polytempo_TempoMeasurement.h"

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
            int height = 100;
            int width  = 360;

            Polytempo_ComposerApplication* const app = dynamic_cast<Polytempo_ComposerApplication*>(JUCEApplication::getInstance());
            
            centreAroundComponent(&app->getMainView(), width, height);
            
            Rectangle<int> rect = getBounds();
            
            String okString = "Ok";
            String cancelString = "Cancel";
  
            contentComponent = new Component();
            contentComponent->setBounds(rect);

            okButton = new TextButton(okString);
            okButton->setBounds(rect.getWidth()-80, rect.getHeight()-30, 60, 20);
            okButton->addListener(this);
            okButton->setWantsKeyboardFocus(false);
            contentComponent->addAndMakeVisible(okButton);

            cancelButton = new TextButton(cancelString);
            cancelButton->setBounds(rect.getWidth()-150, rect.getHeight()-30, 60, 20);
            cancelButton->addListener(this);
            cancelButton->setWantsKeyboardFocus(false);
            contentComponent->addAndMakeVisible(cancelButton);

            setContentOwned(contentComponent, false);
            setBounds(rect);
            setUsingNativeTitleBar(true);
        }
        
        ~Basic()
        {
            contentComponent->deleteAllChildren();
        }
        
        void show()
        {
            setWantsKeyboardFocus(false);
            //contentComponent->grabKeyboardFocus();
            runModalLoop();
        }
        
        void setOkString(const String string)
        {
            okButton->setButtonText(string);
            // TODO: recalculate width and stuff
        }
        
        void enableOkButton(bool enabled)
        {
            okButton->setEnabled(enabled);
        }
        
        virtual void perform()
        {
            DBG("do it!");
        }
        
        void buttonClicked(Button *button)
        {
            if(button == cancelButton)
            {
                setVisible(false);
            }
            if(button == okButton)
            {
                perform();
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
                perform();
                setVisible(false);
                //Polytempo_Composition::getInstance()->updateGUI(); // repaint everything
               return true;
            }
            
            return false;
        }
        
    protected:
        Component *contentComponent;
        TextButton *okButton;
        TextButton *cancelButton;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Basic)
    };
    
    
    class InsertControlPoint : public Basic
    {
    public:
        InsertControlPoint()
        {
            setName("Insert Control Point");
            setOkString("Add");
            enableOkButton(false);
            
            contentComponent->addAndMakeVisible(timeTextbox = new Polytempo_Textbox("Time [s]"));
            timeTextbox->setBounds(20,25,80,18);
            timeTextbox->setText("0", dontSendNotification);
//            timeTextbox->setEscapeAndReturnKeysConsumed(false);
            timeTextbox->setInputRestrictions(0,"0123456789.");
            timeTextbox->addListener(this);
            
            contentComponent->addAndMakeVisible(positionTextbox = new Polytempo_Textbox("Position"));
            positionTextbox->setBounds(120,25,80,18);
            positionTextbox->setText("0", dontSendNotification);
//            positionTextbox->setEscapeAndReturnKeysConsumed(false);
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
        }
        
        void textEditorTextChanged(TextEditor &)
        {
            Polytempo_Sequence* sequence = Polytempo_Composition::getInstance()->getSelectedSequence();
            
            enableOkButton(sequence->validateNewControlPointPosition(timeTextbox->getText().getFloatValue(), Rational(positionTextbox->getText())));
        }
        
        void labelTextChanged (Label* /*label*/)
        {
            Polytempo_Sequence* sequence = Polytempo_Composition::getInstance()->getSelectedSequence();
            
            enableOkButton(sequence->validateNewControlPointPosition(timeTextbox->getText().getFloatValue(), Rational(positionTextbox->getText())));
        }
 
        void perform()
        {
            Polytempo_Sequence* sequence = Polytempo_Composition::getInstance()->getSelectedSequence();
            
            sequence->addControlPoint(timeTextbox->getText().getFloatValue(),
                                      Rational(positionTextbox->getText()),
                                      Polytempo_TempoMeasurement::encodeTempoFromUI((tempoInTextbox->getText().getFloatValue())),
                                      Polytempo_TempoMeasurement::encodeTempoFromUI(tempoOutTextbox->getText().getFloatValue()));
            
            Polytempo_Composition::getInstance()->updateContent(); // repaint everything
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
            setName("Shift Selected Control Points");
            setOkString("Ok");

            Polytempo_Composition* composition = Polytempo_Composition::getInstance();
            Array<int> *indices = composition->getSelectedControlPointIndices();
            firstTime = composition->getSelectedSequence()->getControlPoints()->getUnchecked(indices->getFirst())->time;
            lastTime  = composition->getSelectedSequence()->getControlPoints()->getUnchecked(indices->getLast())->time;
            deltaTime = 0;

            contentComponent->addAndMakeVisible(deltaTextbox = new Polytempo_Textbox("shift points by"));
            deltaTextbox->setBounds(20,25,90,18);
            deltaTextbox->setText(String(deltaTime), dontSendNotification);
            deltaTextbox->setInputRestrictions(0,"0123456789.-");
            deltaTextbox->addListener(this);
            
            contentComponent->addAndMakeVisible(firstTimeTextbox = new Polytempo_Textbox("align first point with"));
            firstTimeTextbox->setBounds(135,25,90,18);
            firstTimeTextbox->setText(String(firstTime), dontSendNotification);
            firstTimeTextbox->setInputRestrictions(0,"0123456789.-");
            firstTimeTextbox->addListener(this);

            contentComponent->addAndMakeVisible(lastTimeTextbox = new Polytempo_Textbox("align last point with"));
            lastTimeTextbox->setBounds(250,25,90,18);
            lastTimeTextbox->setText(String(lastTime), dontSendNotification);
            lastTimeTextbox->setInputRestrictions(0,"0123456789.-");
            lastTimeTextbox->addListener(this);
        }
        
        void labelTextChanged (Label* label)
        {
            if(label == deltaTextbox)          deltaTime = label->getText().getFloatValue();
            else if(label == firstTimeTextbox) deltaTime = label->getText().getFloatValue() - firstTime;
            else if(label == lastTimeTextbox)  deltaTime = label->getText().getFloatValue() - lastTime;

            deltaTextbox->setText(String(deltaTime), dontSendNotification);
            firstTimeTextbox->setText(String(firstTime + deltaTime), dontSendNotification);
            lastTimeTextbox->setText(String(lastTime + deltaTime), dontSendNotification);
        }
 
        void perform()
        {
            Polytempo_Composition* composition = Polytempo_Composition::getInstance();
            Polytempo_Sequence* sequence = composition->getSelectedSequence();
            
            sequence->shiftControlPoints(composition->getSelectedControlPointIndices(), deltaTime);            
        }
        
    private:
        Polytempo_Textbox *deltaTextbox;
        Polytempo_Textbox *firstTimeTextbox;
        Polytempo_Textbox *lastTimeTextbox;

        float firstTime;
        float lastTime;
        float deltaTime;
    };
    
    class ExportSequences : public Basic
    {
    public:
        ExportSequences(int num)
        {
            if(num == 1) setName("Export Selected Sequence");
            else         setName("Export All Sequences");
            setOkString("Export");
            enableOkButton(true);
            
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
            

            Polytempo_ComposerApplication* const app = dynamic_cast<Polytempo_ComposerApplication*>(JUCEApplication::getInstance());
            
            centreAroundComponent(&app->getMainView(), getWidth(), 200);
            
            okButton->setBounds    (getWidth()-80,  getHeight()-30, 60, 20);
            cancelButton->setBounds(getWidth()-150, getHeight()-30, 60, 20);

        }
        
        void textEditorTextChanged(TextEditor &)
        {}
        
        void labelTextChanged (Label*)
        {}
        
        void perform()
        {
            Polytempo_Composition *composition = Polytempo_Composition::getInstance();
            
            if     (tbPlain->getToggleState() == true)     composition->exportAsPlainText();
            else if(tbLisp->getToggleState() == true)      composition->exportAsLispList();
            else if(tbC->getToggleState() == true)         composition->exportAsCArray();
            else if(tbPolytempo->getToggleState() == true) composition->exportAsPolytempoScore();
        }
        
    private:
        ToggleButton *tbPlain, *tbLisp, *tbC, *tbPolytempo;
    };
}


#endif  // __Polytempo_DialogWindows__
