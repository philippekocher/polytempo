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
            tempoInTextbox->setText("0.25", dontSendNotification);
            tempoInTextbox->addListener(this);

            contentComponent->addAndMakeVisible(tempoOutTextbox = new Polytempo_Textbox("Tempo Out"));
            tempoOutTextbox->setBounds(290,25,50,18);
            tempoOutTextbox->setText("0.25", dontSendNotification);
            tempoOutTextbox->addListener(this);
        }
        
        void textEditorTextChanged(TextEditor &)
        {
            Polytempo_Sequence* sequence = Polytempo_Composition::getInstance()->getSelectedSequence();
            
            enableOkButton(sequence->validateNewControlPointPosition(timeTextbox->getText().getIntValue(), Rational(positionTextbox->getText())));
        }
        
        void labelTextChanged (Label* /*label*/)
        {
            Polytempo_Sequence* sequence = Polytempo_Composition::getInstance()->getSelectedSequence();
            
            enableOkButton(sequence->validateNewControlPointPosition(timeTextbox->getText().getIntValue(), Rational(positionTextbox->getText())));
        }
 
        void perform()
        {
            Polytempo_Sequence* sequence = Polytempo_Composition::getInstance()->getSelectedSequence();
            
            sequence->addControlPoint(timeTextbox->getText().getIntValue(),
                                      Rational(positionTextbox->getText()),
                                      tempoInTextbox->getText().getFloatValue(),
                                      tempoOutTextbox->getText().getFloatValue());
            
            Polytempo_Composition::getInstance()->updateContent(); // repaint everything
        }
        
    private:
        Polytempo_Textbox *timeTextbox;
        Polytempo_Textbox *positionTextbox;
        Polytempo_Textbox *tempoInTextbox;
        Polytempo_Textbox *tempoOutTextbox;
    };
    
    
//    class AddEventPattern : public Basic
//    {
//    public:
//        AddEventPattern()
//        {
//            setName("Add Event Pattern");
//            setOkString("Add");
//            enableOkButton(true);
//            
//            label1.setText("Pattern", dontSendNotification);
//            label1.setEditable(false);
//            label1.setBounds(20,20,150,15);
//            contentComponent->addAndMakeVisible(label1);
//            
//            editor1.setBounds(20,35,150,15);
//            editor1.setText("4/4", dontSendNotification);
//            editor1.setEscapeAndReturnKeysConsumed(false);
//            editor1.setSelectAllWhenFocused(true);
//            editor1.setCaretVisible(false);
//            editor1.setInputRestrictions(0,"0123456789/+");
//            editor1.addListener(this);
//            contentComponent->addAndMakeVisible(editor1);
//            
//            label2.setText("Repeats", dontSendNotification);
//            label2.setEditable(false);
//            label2.setBounds(170,20,50,15);
//            contentComponent->addAndMakeVisible(label2);
//            
//            editor2.setBounds(170,35,50,15);
//            editor2.setText("1", dontSendNotification);
//            editor2.setEscapeAndReturnKeysConsumed(false);
//            editor2.setSelectAllWhenFocused(true);
//            editor2.setCaretVisible(false);
//            editor2.setInputRestrictions(0,"0123456789");
//            editor2.addListener(this);
//            contentComponent->addAndMakeVisible(editor2);
//
//            label3.setText("Counter", dontSendNotification);
//            label3.setEditable(false);
//            label3.setBounds(230,20,50,15);
//            contentComponent->addAndMakeVisible(label3);
//            
//            Polytempo_Sequence* sequence = Polytempo_Composition::getInstance()->getSelectedSequence();
//            editor3.setBounds(230,35,50,15);
//            editor3.setText(String(sequence->getCurrentCounter()), dontSendNotification);
//            editor3.setEscapeAndReturnKeysConsumed(false);
//            editor3.setSelectAllWhenFocused(true);
//            editor3.setCaretVisible(false);
//            editor3.setInputRestrictions(0,"0123456789 ");
//            editor3.addListener(this);
//            contentComponent->addAndMakeVisible(editor3);
//
//            label4.setText("Marker", dontSendNotification);
//            label4.setEditable(false);
//            label4.setBounds(290,20,50,15);
//            contentComponent->addAndMakeVisible(label4);
//            
//            editor4.setBounds(290,35,50,15);
//            editor4.setEscapeAndReturnKeysConsumed(false);
//            editor4.setSelectAllWhenFocused(true);
//            editor4.setCaretVisible(false);
//            editor4.addListener(this);
//            contentComponent->addAndMakeVisible(editor4);
//        }
//        
//        void textEditorTextChanged(TextEditor &)
//        {}
//        
//        void labelTextChanged (Label* label)
//        {}
//        
//        void perform()
//        {
//            Polytempo_Sequence* sequence = Polytempo_Composition::getInstance()->getSelectedSequence();
//            sequence->addEventPattern(editor1.getText(), editor2.getText().getIntValue(), editor3.getText(), editor4.getText());
//            Polytempo_Composition::getInstance()->updateContent(); // repaint everything
//        }
//        
//    private:
//        Label label1, label2, label3, label4;
//        TextEditor editor1, editor2, editor3, editor4;
//    };
//    
    
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
