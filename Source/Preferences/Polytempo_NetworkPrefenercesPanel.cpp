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

#include "Polytempo_NetworkPreferencesPanel.h"
#include "Polytempo_StoredPreferences.h"
#include "../Application/PolytempoNetwork/Polytempo_NetworkApplication.h"
#include "../Audio+Midi/Polytempo_AudioDeviceManager.h"
#include "../Audio+Midi/Polytempo_AudioClick.h"
#include "../Audio+Midi/Polytempo_MidiClick.h"


/** A TextButton that pops up a colour chooser to change its colours. */
class ColourChangeButton : public TextButton,
                           public ChangeListener
{
public:
    ColourChangeButton()
    : TextButton("click to change colour...")
    {
        setSize(10, 24);
        changeWidthToFitText();
    }
    
    ~ColourChangeButton() {}
    
    void clicked()
    {
#if JUCE_MODAL_LOOPS_PERMITTED
        colourSelector = new ColourSelector(juce::ColourSelector::showColourAtTop | juce::ColourSelector::showSliders | juce::ColourSelector::showColourspace);
        
        colourSelector->setCurrentColour(findColour(TextButton::buttonColourId));
        colourSelector->addChangeListener(this);
        colourSelector->setColour(ColourSelector::backgroundColourId, Colours::transparentBlack);
        colourSelector->setSize(300, 400);
        
        CallOutBox callOut(*colourSelector, this->getBounds(), nullptr);
        callOut.setArrowSize(0);
        callOut.runModalLoop();
#endif
    }
    
    void changeListenerCallback(ChangeBroadcaster* source)
    {
        ColourSelector* cs = dynamic_cast <ColourSelector*>(source);
        
        setColour(TextButton::buttonColourId, cs->getCurrentColour());
    }
    
private:
    ScopedPointer<ColourSelector> colourSelector;

};

//------------------------------------------------------------------------------
#pragma mark -
#pragma mark General Preferences Page

class GeneralPreferencesPage : public Component, Button::Listener, TextEditor::Listener
{
    TextEditor *defaultFilePath;
    TextButton *chooseDefaultFile;
    Label      *defaultFilePathLabel;
    ToggleButton* fullScreenToggle;

    ToggleButton* broadcastCommandsToggle;

    
public:
    GeneralPreferencesPage()
    {
        addAndMakeVisible(defaultFilePath = new TextEditor());
        defaultFilePath->setText(Polytempo_StoredPreferences::getInstance()->getProps().getValue("defaultFilePath"));
        defaultFilePath->setBorder(BorderSize<int>(1));
        defaultFilePath->setColour(TextEditor::outlineColourId, Colours::grey);
        defaultFilePath->addListener(this);
        
        addAndMakeVisible(defaultFilePathLabel = new Label(String::empty, L"Default Score File"));
        defaultFilePathLabel->setFont(Font(15.0000f, Font::plain));
        defaultFilePathLabel->setJustificationType(Justification::centredLeft);
        defaultFilePathLabel->setEditable(false, false, false);

        addAndMakeVisible(chooseDefaultFile = new TextButton("Choose.."));
        chooseDefaultFile->addListener(this);

        addAndMakeVisible(fullScreenToggle = new ToggleButton(String::empty));
        fullScreenToggle->setButtonText(L"Full Screen");
        fullScreenToggle->setToggleState(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("fullScreen"), dontSendNotification);
        fullScreenToggle->addListener(this);

        addAndMakeVisible(broadcastCommandsToggle = new ToggleButton(String::empty));
        broadcastCommandsToggle->setButtonText(L"Broadcast Scheduler Commands");
        broadcastCommandsToggle->setToggleState(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("broadcastSchedulerCommands"), dontSendNotification);
        broadcastCommandsToggle->addListener(this);
}
    
    ~GeneralPreferencesPage()
    {
        deleteAllChildren();
    }
    
    void resized()
    {
        defaultFilePath->setBounds     (20,  70, proportionOfWidth(0.9f), 24);
        defaultFilePathLabel->setBounds(18,  35, 200, 24);
        chooseDefaultFile->setBounds   (20, 100, proportionOfWidth(0.2f), 24);
        
        fullScreenToggle->setBounds    (getWidth() - proportionOfWidth(0.2f) - 20, 100, proportionOfWidth(0.2f), 24);
        
        broadcastCommandsToggle->setBounds(20,  200, proportionOfWidth(0.9f), 24);
    }
    
    /* text editor & button listener
     --------------------------------------- */
    void textEditorTextChanged (TextEditor& editor)
    {
        DBG(editor.getText());
        Polytempo_StoredPreferences::getInstance()->getProps().setValue("defaultFilePath", editor.getText());

    }
    
    void textEditorReturnKeyPressed (TextEditor& editor)
    {}
    
    void textEditorEscapeKeyPressed (TextEditor&)
    {}
    
    void textEditorFocusLost (TextEditor&)
    {}
    
    
    void buttonClicked(Button* button)
    {
        if(button == chooseDefaultFile)
        {
            File directory (Polytempo_StoredPreferences::getInstance()->getProps().getValue("scoreFileDirectory"));
            FileChooser fileChooser ("Open Score File", directory, "*.json;*.ptsco");
            
            if(fileChooser.browseForFileToOpen())
            {
                Polytempo_StoredPreferences::getInstance()->getProps().setValue ("defaultFilePath", fileChooser.getResult().getFullPathName());
                defaultFilePath->setText(fileChooser.getResult().getFullPathName());
                
                Polytempo_NetworkApplication* const app = dynamic_cast<Polytempo_NetworkApplication*>(JUCEApplication::getInstance());
                app->openScoreFile(fileChooser.getResult());
            }
        }
        else if(button == fullScreenToggle)
        {
            Polytempo_StoredPreferences::getInstance()->getProps().setValue("fullScreen", button->getToggleState());
        }
        else if(button == broadcastCommandsToggle)
        {
            Polytempo_StoredPreferences::getInstance()->getProps().setValue("broadcastSchedulerCommands", button->getToggleState());
         }
    }
    
    void buttonStateChanged(Button* button)
    {}

};

//------------------------------------------------------------------------------
#pragma mark -
#pragma mark Visual Preferences Page
    
class VisualPreferencesPage : public Component, Button::Listener, Slider::Listener
   {
    ToggleButton* showVisualMetro;
    Label*  stripWidthLabel;
    Slider* stripWidthSlider;
    Label*  stripLengthLabel;
    Slider* stripLengthSlider;
    Label*  stripNormalColourLabel;
    ColourChangeButton *stripNormalColourSelector;
    Label*  stripCueColourLabel;
    ColourChangeButton *stripCueColourSelector;
    Label*  stripBackgroundColourLabel;
    ColourChangeButton *stripBackgroundColourSelector;
    Label*  stripFrameColourLabel;
    ColourChangeButton *stripFrameColourSelector;
    
    ToggleButton* showAuxiliaryView;
    
public:
    VisualPreferencesPage()
    {
        addAndMakeVisible(showVisualMetro = new ToggleButton(String::empty));
        showVisualMetro->setButtonText(L"Show Visual Metro");
        showVisualMetro->setToggleState(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("showVisualMetro"), dontSendNotification);
        showVisualMetro->addListener(this);
        
        addAndMakeVisible(stripWidthLabel = new Label(String::empty, L"Width"));
        stripWidthLabel->setFont(Font(15.0000f, Font::plain));
        stripWidthLabel->setJustificationType(Justification::centredLeft);
        stripWidthLabel->setEditable(false, false, false);
        stripWidthLabel->setEnabled(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("showVisualMetro"));
        
        addAndMakeVisible(stripWidthSlider = new Slider(String::empty));
        stripWidthSlider->setRange(5, 100, 1);
        stripWidthSlider->setSliderStyle(Slider::LinearHorizontal);
        stripWidthSlider->setTextBoxStyle(Slider::TextBoxLeft, false, 80, 20);
        stripWidthSlider->addListener(this);
        stripWidthSlider->setValue(Polytempo_StoredPreferences::getInstance()->getProps().getIntValue("stripWidth"));
        stripWidthSlider->setEnabled(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("showVisualMetro"));
        
        addAndMakeVisible(stripNormalColourLabel = new Label(String::empty, L"Normal Beat"));
        stripNormalColourLabel->setFont(Font(15.0000f, Font::plain));
        stripNormalColourLabel->setJustificationType(Justification::centredLeft);
        stripNormalColourLabel->setEditable(false, false, false);
        stripNormalColourLabel->setEnabled(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("showVisualMetro"));
        
        addAndMakeVisible(stripNormalColourSelector = new ColourChangeButton());
        stripNormalColourSelector->setEnabled(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("showVisualMetro"));
        stripNormalColourSelector->setColour(TextButton::buttonColourId, Colour::fromString(Polytempo_StoredPreferences::getInstance()->getProps().getValue("stripNormalColour")));
        stripNormalColourSelector->addListener(this);
        stripNormalColourSelector->setEnabled(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("showVisualMetro"));
        
        addAndMakeVisible(stripCueColourLabel = new Label(String::empty, L"Cue In"));
        stripCueColourLabel->setFont(Font(15.0000f, Font::plain));
        stripCueColourLabel->setJustificationType(Justification::centredLeft);
        stripCueColourLabel->setEditable(false, false, false);
        stripCueColourLabel->setEnabled(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("showVisualMetro"));
        
        addAndMakeVisible(stripCueColourSelector = new ColourChangeButton());
        stripCueColourSelector->setEnabled(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("showVisualMetro"));
        stripCueColourSelector->setColour(TextButton::buttonColourId, Colour::fromString(Polytempo_StoredPreferences::getInstance()->getProps().getValue("stripCueColour")));
        stripCueColourSelector->addListener(this);
        stripCueColourSelector->setEnabled(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("showVisualMetro"));
        
        addAndMakeVisible(stripBackgroundColourLabel = new Label(String::empty, L"Background"));
        stripBackgroundColourLabel->setFont(Font(15.0000f, Font::plain));
        stripBackgroundColourLabel->setJustificationType(Justification::centredLeft);
        stripBackgroundColourLabel->setEditable(false, false, false);
        stripBackgroundColourLabel->setEnabled(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("showVisualMetro"));
        
        addAndMakeVisible(stripBackgroundColourSelector = new ColourChangeButton());
        stripBackgroundColourSelector->setEnabled(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("showVisualMetro"));
        stripBackgroundColourSelector->setColour(TextButton::buttonColourId, Colour::fromString(Polytempo_StoredPreferences::getInstance()->getProps().getValue("stripBackgroundColour")));
        stripBackgroundColourSelector->addListener(this);
        stripBackgroundColourSelector->setEnabled(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("showVisualMetro"));
        
        addAndMakeVisible(stripFrameColourLabel = new Label(String::empty, L"Frame"));
        stripFrameColourLabel->setFont(Font(15.0000f, Font::plain));
        stripFrameColourLabel->setJustificationType(Justification::centredLeft);
        stripFrameColourLabel->setEditable(false, false, false);
        stripFrameColourLabel->setEnabled(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("showVisualMetro"));
        
        addAndMakeVisible(stripFrameColourSelector = new ColourChangeButton());
        stripFrameColourSelector->setEnabled(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("showVisualMetro"));
        stripFrameColourSelector->setColour(TextButton::buttonColourId, Colour::fromString(Polytempo_StoredPreferences::getInstance()->getProps().getValue("stripFrameColour")));
        stripFrameColourSelector->addListener(this);
        stripFrameColourSelector->setEnabled(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("showVisualMetro"));
    
        addAndMakeVisible(showAuxiliaryView = new ToggleButton(String::empty));
        showAuxiliaryView->setButtonText(L"Show Auxiliary View");
        showAuxiliaryView->setToggleState(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("showAuxiliaryView"), dontSendNotification);
        showAuxiliaryView->addListener(this);
    }
    
    ~VisualPreferencesPage()
    {        
        deleteAllChildren();
    }
    
    void resized()
    {
        showVisualMetro->setBounds   (20,  50, proportionOfWidth(0.9f), 24);
        stripWidthLabel->setBounds   (20,  80, 90, 24);
        stripWidthSlider->setBounds  (120, 80, proportionOfWidth(0.7f), 24);
        
        stripNormalColourLabel->setBounds       (20,  120, 90, 24);
        stripNormalColourSelector->setBounds    (120, 120, getWidth() - 140, 24);
        stripCueColourLabel->setBounds          (20,  150, 90, 24);
        stripCueColourSelector->setBounds       (120, 150, getWidth() - 140, 24);
        stripBackgroundColourLabel->setBounds   (20,  180, 90, 24);
        stripBackgroundColourSelector->setBounds(120, 180, getWidth() - 140, 24);
        stripFrameColourLabel->setBounds        (20,  210, 90, 24);
        stripFrameColourSelector->setBounds     (120, 210, getWidth() - 140, 24);

        showAuxiliaryView->setBounds (20,  280, proportionOfWidth(0.9f), 24);
    }
    
    /* slider & button listener
     --------------------------------------- */
    void sliderValueChanged(Slider* slider)
    {
        if(slider == stripWidthSlider)
            Polytempo_StoredPreferences::getInstance()->getProps().setValue("stripWidth", slider->getValue());
        
    }
    
    void buttonClicked(Button* button)
    {
        if(button == showVisualMetro)
        {
            Polytempo_StoredPreferences::getInstance()->getProps().setValue("showVisualMetro", button->getToggleState());
            stripWidthLabel->setEnabled(button->getToggleState());
            stripWidthSlider->setEnabled(button->getToggleState());
            stripNormalColourLabel->setEnabled(button->getToggleState());
            stripNormalColourSelector->setEnabled(button->getToggleState());
            stripBackgroundColourLabel->setEnabled(button->getToggleState());
            stripBackgroundColourSelector->setEnabled(button->getToggleState());
            stripFrameColourLabel->setEnabled(button->getToggleState());
            stripFrameColourSelector->setEnabled(button->getToggleState());
        }
        else if(button == stripNormalColourSelector)
        {
            Polytempo_StoredPreferences::getInstance()->getProps().setValue("stripNormalColour", button->findColour(TextButton::buttonColourId).toString());
        }
        else if(button == stripBackgroundColourSelector)
        {
            Polytempo_StoredPreferences::getInstance()->getProps().setValue("stripBackgroundColour", button->findColour(TextButton::buttonColourId).toString());
        }
        else if(button == stripFrameColourSelector)
        {
            Polytempo_StoredPreferences::getInstance()->getProps().setValue("stripFrameColour", button->findColour(TextButton::buttonColourId).toString());
        }
        else if(button == showAuxiliaryView)
        {
            Polytempo_StoredPreferences::getInstance()->getProps().setValue("showAuxiliaryView", button->getToggleState());
        }
    }
    
    void buttonStateChanged(Button* button)
    {}
};

//------------------------------------------------------------------------------
#pragma mark -
#pragma mark Audio Preferences Page

class AudioPreferencesPage : public Component,
Button::Listener,
Slider::Listener,
TextEditor::Listener,
ComboBox::Listener,
ChangeListener
{
    ToggleButton *audioClick;
    Label        *audioDownbeatPitchLabel;
    TextEditor   *audioDownbeatPitch;
    Label        *audioDownbeatVolumeLabel;
    Slider       *audioDownbeatVolumeSlider;
    Label        *audioBeatPitchLabel;
    TextEditor   *audioBeatPitch;
    Label        *audioBeatVolumeLabel;
    Slider       *audioBeatVolumeSlider;
    Label        *audioCuePitchLabel;
    TextEditor   *audioCuePitch;
    Label        *audioCueVolumeLabel;
    Slider       *audioCueVolumeSlider;
    
    AudioDeviceManager& audioDeviceManager;
    AudioDeviceSelectorComponent* deviceSelector;
    ScopedPointer<XmlElement> audioDeviceManagerState;
    
public:
    AudioPreferencesPage()
    :audioDeviceManager(Polytempo_AudioDeviceManager::getSharedAudioDeviceManager())
    {
        /* audio main toggle
         -------------------------------------------------- */
        
        addAndMakeVisible(audioClick = new ToggleButton(String::empty));
        audioClick->setButtonText(L"Activate Audio Click");
        audioClick->setToggleState(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("audioClick"), dontSendNotification);
        audioClick->addListener(this);
        
        /* audio downbeat
         -------------------------------------------------- */
        
        addAndMakeVisible(audioDownbeatPitchLabel = new Label(String::empty, L"Downbeat Pitch"));
        audioDownbeatPitchLabel->setFont(Font(15.0000f, Font::plain));
        audioDownbeatPitchLabel->setJustificationType(Justification::centredLeft);
        audioDownbeatPitchLabel->setEditable(false, false, false);
        audioDownbeatPitchLabel->setEnabled(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("audioClick"));
        
        addAndMakeVisible(audioDownbeatPitch = new TextEditor());
        audioDownbeatPitch->setText(String(Polytempo_StoredPreferences::getInstance()->getProps().getIntValue("audioDownbeatPitch")));
        audioDownbeatPitch->setEnabled(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("audioClick"));
        audioDownbeatPitch->addListener(this);
        
        addAndMakeVisible(audioDownbeatVolumeLabel = new Label(String::empty, L"Downbeat Volume"));
        audioDownbeatVolumeLabel->setFont(Font(15.0000f, Font::plain));
        audioDownbeatVolumeLabel->setJustificationType(Justification::centredLeft);
        audioDownbeatVolumeLabel->setEditable(false, false, false);
        audioDownbeatVolumeLabel->setColour(TextEditor::textColourId, Colours::black);
        audioDownbeatVolumeLabel->setColour(TextEditor::backgroundColourId, Colour(0x0));
        audioDownbeatVolumeLabel->setEnabled(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("audioClick"));
        
        addAndMakeVisible(audioDownbeatVolumeSlider = new Slider(String::empty));
        audioDownbeatVolumeSlider->setRange(0, 1, 0.01);
        audioDownbeatVolumeSlider->setSliderStyle(Slider::LinearHorizontal);
        audioDownbeatVolumeSlider->setTextBoxStyle(Slider::TextBoxLeft, false, 80, 20);
        audioDownbeatVolumeSlider->addListener(this);
        audioDownbeatVolumeSlider->setValue(Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("audioDownbeatVolume"));
        audioDownbeatVolumeSlider->setEnabled(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("audioClick"));
        
        /* audio beat
         -------------------------------------------------- */
        
        addAndMakeVisible(audioBeatPitchLabel = new Label(String::empty, L"Beat Pitch"));
        audioBeatPitchLabel->setFont(Font(15.0000f, Font::plain));
        audioBeatPitchLabel->setJustificationType(Justification::centredLeft);
        audioBeatPitchLabel->setEditable(false, false, false);
        audioBeatPitchLabel->setEnabled(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("audioClick"));
        
        addAndMakeVisible(audioBeatPitch = new TextEditor());
        audioBeatPitch->setText(String(Polytempo_StoredPreferences::getInstance()->getProps().getIntValue("audioBeatPitch")));
        audioBeatPitch->setEnabled(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("audioClick"));
        audioBeatPitch->addListener(this);
        
        addAndMakeVisible(audioBeatVolumeLabel = new Label(String::empty, L"Beat Volume"));
        audioBeatVolumeLabel->setFont(Font(15.0000f, Font::plain));
        audioBeatVolumeLabel->setJustificationType(Justification::centredLeft);
        audioBeatVolumeLabel->setEditable(false, false, false);
        audioBeatVolumeLabel->setEnabled(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("audioClick"));
        
        addAndMakeVisible(audioBeatVolumeSlider = new Slider(String::empty));
        audioBeatVolumeSlider->setRange(0, 1, 0.01);
        audioBeatVolumeSlider->setSliderStyle(Slider::LinearHorizontal);
        audioBeatVolumeSlider->setTextBoxStyle(Slider::TextBoxLeft, false, 80, 20);
        audioBeatVolumeSlider->addListener(this);
        audioBeatVolumeSlider->setValue(Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("audioBeatVolume"));
        audioBeatVolumeSlider->setEnabled(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("audioClick"));
        
        addAndMakeVisible(deviceSelector = new AudioDeviceSelectorComponent(audioDeviceManager, 0, 0, 0, 2, false, true, true, false));
        audioDeviceManager.addChangeListener(this);
    
        /* audio cue in
         -------------------------------------------------- */
        
        addAndMakeVisible(audioCuePitchLabel = new Label(String::empty, L"Cue Pitch"));
        audioCuePitchLabel->setFont(Font(15.0000f, Font::plain));
        audioCuePitchLabel->setJustificationType(Justification::centredLeft);
        audioCuePitchLabel->setEditable(false, false, false);
        audioCuePitchLabel->setEnabled(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("audioClick"));
        
        addAndMakeVisible(audioCuePitch = new TextEditor());
        audioCuePitch->setText(String(Polytempo_StoredPreferences::getInstance()->getProps().getIntValue("audioCuePitch")));
        audioCuePitch->setEnabled(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("audioClick"));
        audioCuePitch->addListener(this);
        
        addAndMakeVisible(audioCueVolumeLabel = new Label(String::empty, L"Cue Volume"));
        audioCueVolumeLabel->setFont(Font(15.0000f, Font::plain));
        audioCueVolumeLabel->setJustificationType(Justification::centredLeft);
        audioCueVolumeLabel->setEditable(false, false, false);
        audioCueVolumeLabel->setEnabled(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("audioClick"));
        
        addAndMakeVisible(audioCueVolumeSlider = new Slider(String::empty));
        audioCueVolumeSlider->setRange(0, 1, 0.01);
        audioCueVolumeSlider->setSliderStyle(Slider::LinearHorizontal);
        audioCueVolumeSlider->setTextBoxStyle(Slider::TextBoxLeft, false, 80, 20);
        audioCueVolumeSlider->addListener(this);
        audioCueVolumeSlider->setValue(Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("audioCueVolume"));
        audioCueVolumeSlider->setEnabled(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("audioClick"));
        
        addAndMakeVisible(deviceSelector = new AudioDeviceSelectorComponent(audioDeviceManager, 0, 0, 0, 2, false, false, true, false));
        audioDeviceManager.addChangeListener(this);
}
    
    ~AudioPreferencesPage()
    {
        deleteAllChildren();
        audioDeviceManager.removeChangeListener(this);
        audioDeviceManagerState = nullptr;
    }
    
    void resized() override
    {
        audioClick->setBounds              (20, 20, getWidth() - 20, 24);
        
        audioDownbeatPitchLabel->setBounds (20, 50, 100, 24);
        audioDownbeatPitch->setBounds      (120,50,  80, 20);
        audioDownbeatVolumeLabel->setBounds(20, 70, 100, 24);
        audioDownbeatVolumeSlider->setBounds(120,70, getWidth() - 140, 24);
        
        audioBeatPitchLabel->setBounds     (20, 100, 100, 24);
        audioBeatPitch->setBounds          (120,100,  80, 20);
        audioBeatVolumeLabel->setBounds    (20, 120, 100, 24);
        audioBeatVolumeSlider->setBounds   (120,120, getWidth() - 140, 24);
        
        audioCuePitchLabel->setBounds      (20, 150, 100, 24);
        audioCuePitch->setBounds           (120,150,  80, 20);
        audioCueVolumeLabel->setBounds     (20, 170, 100, 24);
        audioCueVolumeSlider->setBounds    (120,170, getWidth() - 140, 24);
        
        deviceSelector->setBounds          (20, 220, getWidth() - 20, 300);
    }
    
    /* text editor listener
     --------------------------------------- */
    void textEditorTextChanged(TextEditor& textEditor) override
    {}
    
    void textEditorReturnKeyPressed(TextEditor& textEditor) override
    {
        textEditor.moveKeyboardFocusToSibling(true);
    }
    
    void textEditorEscapeKeyPressed(TextEditor& textEditor) override
    {}
    
    void textEditorFocusLost(TextEditor& textEditor) override
    {
        int value;
        
        if(&textEditor == audioDownbeatPitch || &textEditor == audioBeatPitch ||
           &textEditor == audioCuePitch)
        {
            value = textEditor.getText().getIntValue();
            value = value > 127 ? 127 : value < 0 ? 0 : value;
            textEditor.setText(String(value));
        }
        
        if(&textEditor == audioDownbeatPitch)
        {
            Polytempo_StoredPreferences::getInstance()->getProps().setValue("audioDownbeatPitch", value);
            Polytempo_AudioClick::getInstance()->downbeatPitch = value;
        }
        else if(&textEditor == audioBeatPitch)
        {
            Polytempo_StoredPreferences::getInstance()->getProps().setValue("audioBeatPitch", value);
            Polytempo_AudioClick::getInstance()->beatPitch = value;
        }
        else if(&textEditor == audioCuePitch)
        {
            Polytempo_StoredPreferences::getInstance()->getProps().setValue("audioCuePitch", value);
            Polytempo_AudioClick::getInstance()->cuePitch = value;
        }
    }
    
    
    /* slider listener
     --------------------------------------- */
    void sliderValueChanged(Slider* slider) override
    {
        if(slider == audioDownbeatVolumeSlider)
        {
            Polytempo_StoredPreferences::getInstance()->getProps().setValue("audioDownbeatVolume", slider->getValue());
            Polytempo_AudioClick::getInstance()->downbeatVolume = slider->getValue();
        }
        else if(slider == audioBeatVolumeSlider)
        {
            Polytempo_StoredPreferences::getInstance()->getProps().setValue("audioBeatVolume", slider->getValue());
            Polytempo_AudioClick::getInstance()->beatVolume = slider->getValue();
        }
        else if(slider == audioCueVolumeSlider)
        {
            Polytempo_StoredPreferences::getInstance()->getProps().setValue("audioCueVolume", slider->getValue());
            Polytempo_AudioClick::getInstance()->beatVolume = slider->getValue();
        }
    }
    
    /* button listener
     --------------------------------------- */
    void buttonClicked(Button* button) override
    {
        if(button == audioClick)
        {
            Polytempo_StoredPreferences::getInstance()->getProps().setValue("audioClick", button->getToggleState());
            
            audioDownbeatPitchLabel->setEnabled(button->getToggleState());
            audioDownbeatPitch->setEnabled(button->getToggleState());
            audioDownbeatVolumeLabel->setEnabled(button->getToggleState());
            audioDownbeatVolumeSlider->setEnabled(button->getToggleState());
            
            audioBeatPitchLabel->setEnabled(button->getToggleState());
            audioBeatPitch->setEnabled(button->getToggleState());
            audioBeatVolumeLabel->setEnabled(button->getToggleState());
            audioBeatVolumeSlider->setEnabled(button->getToggleState());
            
            audioCuePitchLabel->setEnabled(button->getToggleState());
            audioCuePitch->setEnabled(button->getToggleState());
            audioCueVolumeLabel->setEnabled(button->getToggleState());
            audioCueVolumeSlider->setEnabled(button->getToggleState());
        }
    }
    
    void buttonStateChanged(Button* button) override
    {}
    
    /* combo box listener
     --------------------------------------- */
    
    void comboBoxChanged(ComboBox* box) override
    {}
    
    
    /* change listener
     --------------------------------------- */
    
    void changeListenerCallback(ChangeBroadcaster *source) override
    {
        if(source == &audioDeviceManager)
        {
            audioDeviceManagerState = audioDeviceManager.createStateXml();
            Polytempo_StoredPreferences::getInstance()->getProps().setValue("audioDevice", audioDeviceManagerState);
        }
    }
    
};

//------------------------------------------------------------------------------
#pragma mark -
#pragma mark Midi Preferences Page

class MidiPreferencesPage : public Component,
Button::Listener,
Slider::Listener,
TextEditor::Listener,
ComboBox::Listener,
ChangeListener
{
    ToggleButton *midiClick;
    Label        *midiDownbeatPitchLabel;
    TextEditor   *midiDownbeatPitch;
    Label        *midiDownbeatVelocityLabel;
    Slider       *midiDownbeatVelocitySlider;
    Label        *midiBeatPitchLabel;
    TextEditor   *midiBeatPitch;
    Label        *midiBeatVelocityLabel;
    Slider       *midiBeatVelocitySlider;
    Label        *midiCuePitchLabel;
    TextEditor   *midiCuePitch;
    Label        *midiCueVelocityLabel;
    Slider       *midiCueVelocitySlider;
    Label        *midiOutputDeviceListLabel;
    ComboBox     *midiOutputDeviceList;
    Label        *midiChannelLabel;
    TextEditor   *midiChannel;
    
public:
    MidiPreferencesPage()
    {
        /* midi main toggle
         -------------------------------------------------- */
        
        addAndMakeVisible(midiClick = new ToggleButton(String::empty));
        midiClick->setButtonText(L"Activate Midi Click");
        midiClick->setToggleState(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("midiClick"), dontSendNotification);
        midiClick->addListener(this);
        
        
        /* midi downbeat
         -------------------------------------------------- */
        
        addAndMakeVisible(midiDownbeatPitchLabel = new Label(String::empty, L"Downbeat Pitch"));
        midiDownbeatPitchLabel->setFont(Font(15.0000f, Font::plain));
        midiDownbeatPitchLabel->setJustificationType(Justification::centredLeft);
        midiDownbeatPitchLabel->setEditable(false, false, false);
        midiDownbeatPitchLabel->setEnabled(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("midiClick"));
        
        addAndMakeVisible(midiDownbeatPitch = new TextEditor());
        midiDownbeatPitch->setText(String(Polytempo_StoredPreferences::getInstance()->getProps().getIntValue("midiDownbeatPitch")));
        midiDownbeatPitch->setEnabled(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("midiClick"));
        midiDownbeatPitch->addListener(this);
        
        addAndMakeVisible(midiDownbeatVelocityLabel = new Label(String::empty, L"Downbeat Velocity"));
        midiDownbeatVelocityLabel->setFont(Font(15.0000f, Font::plain));
        midiDownbeatVelocityLabel->setJustificationType(Justification::centredLeft);
        midiDownbeatVelocityLabel->setEditable(false, false, false);
        midiDownbeatVelocityLabel->setEnabled(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("midiClick"));
        
        addAndMakeVisible(midiDownbeatVelocitySlider = new Slider(String::empty));
        midiDownbeatVelocitySlider->setRange(0, 127, 1);
        midiDownbeatVelocitySlider->setSliderStyle(Slider::LinearHorizontal);
        midiDownbeatVelocitySlider->setTextBoxStyle(Slider::TextBoxLeft, false, 80, 20);
        midiDownbeatVelocitySlider->addListener(this);
        midiDownbeatVelocitySlider->setValue(Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("midiDownbeatVelocity"));
        midiDownbeatVelocitySlider->setEnabled(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("midiClick"));
        
        /* midi beat
         -------------------------------------------------- */
        
        addAndMakeVisible(midiBeatPitchLabel = new Label(String::empty, L"Beat Pitch"));
        midiBeatPitchLabel->setFont(Font(15.0000f, Font::plain));
        midiBeatPitchLabel->setJustificationType(Justification::centredLeft);
        midiBeatPitchLabel->setEditable(false, false, false);
        midiBeatPitchLabel->setEnabled(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("midiClick"));
        
        addAndMakeVisible(midiBeatPitch = new TextEditor());
        midiBeatPitch->setText(String(Polytempo_StoredPreferences::getInstance()->getProps().getIntValue("midiBeatPitch")));
        midiBeatPitch->setEnabled(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("midiClick"));
        midiBeatPitch->addListener(this);
        
        addAndMakeVisible(midiBeatVelocityLabel = new Label(String::empty, L"Beat Velocity"));
        midiBeatVelocityLabel->setFont(Font(15.0000f, Font::plain));
        midiBeatVelocityLabel->setJustificationType(Justification::centredLeft);
        midiBeatVelocityLabel->setEditable(false, false, false);
        midiBeatVelocityLabel->setEnabled(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("midiClick"));
        
        addAndMakeVisible(midiBeatVelocitySlider = new Slider(String::empty));
        midiBeatVelocitySlider->setRange(0, 127, 1);
        midiBeatVelocitySlider->setSliderStyle(Slider::LinearHorizontal);
        midiBeatVelocitySlider->setTextBoxStyle(Slider::TextBoxLeft, false, 80, 20);
        midiBeatVelocitySlider->addListener(this);
        midiBeatVelocitySlider->setValue(Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("midiBeatVelocity"));
        midiBeatVelocitySlider->setEnabled(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("midiClick"));
        
        /* midi cue in
         -------------------------------------------------- */
        
        addAndMakeVisible(midiCuePitchLabel = new Label(String::empty, L"Cue Pitch"));
        midiCuePitchLabel->setFont(Font(15.0000f, Font::plain));
        midiCuePitchLabel->setJustificationType(Justification::centredLeft);
        midiCuePitchLabel->setEditable(false, false, false);
        midiCuePitchLabel->setEnabled(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("midiClick"));
        
        addAndMakeVisible(midiCuePitch = new TextEditor());
        midiCuePitch->setText(String(Polytempo_StoredPreferences::getInstance()->getProps().getIntValue("midiCuePitch")));
        midiCuePitch->setEnabled(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("midiClick"));
        midiCuePitch->addListener(this);
        
        addAndMakeVisible(midiCueVelocityLabel = new Label(String::empty, L"Beat Velocity"));
        midiCueVelocityLabel->setFont(Font(15.0000f, Font::plain));
        midiCueVelocityLabel->setJustificationType(Justification::centredLeft);
        midiCueVelocityLabel->setEditable(false, false, false);
        midiCueVelocityLabel->setEnabled(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("midiClick"));
        
        addAndMakeVisible(midiCueVelocitySlider = new Slider(String::empty));
        midiCueVelocitySlider->setRange(0, 127, 1);
        midiCueVelocitySlider->setSliderStyle(Slider::LinearHorizontal);
        midiCueVelocitySlider->setTextBoxStyle(Slider::TextBoxLeft, false, 80, 20);
        midiCueVelocitySlider->addListener(this);
        midiCueVelocitySlider->setValue(Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("midiCueVelocity"));
        midiCueVelocitySlider->setEnabled(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("midiClick"));
        
        /* midi device and channel
         -------------------------------------------------- */
        
        addAndMakeVisible(midiOutputDeviceListLabel = new Label(String::empty, "MIDI Output"));
        midiOutputDeviceListLabel->setFont(Font(15.0000f, Font::plain));
        midiOutputDeviceListLabel->setJustificationType(Justification::centredLeft);
        midiOutputDeviceListLabel->setEnabled(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("midiClick"));
       
        addAndMakeVisible(midiOutputDeviceList = new ComboBox());
        midiOutputDeviceList->setTextWhenNoChoicesAvailable("No MIDI Outputs Enabled");
        const StringArray midiOutputs(MidiOutput::getDevices());
        midiOutputDeviceList->addItemList(midiOutputs, 1);
        midiOutputDeviceList->setEnabled(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("midiClick"));
        midiOutputDeviceList->addListener(this);
        
        // find the device stored in the settings
        juce::StringArray midiDevices = MidiOutput::getDevices();
        int index = midiDevices.indexOf(Polytempo_StoredPreferences::getInstance()->getProps().getValue("midiOutputDevice"));
        if(index < 0) index = 0; // otherwise set first device
        midiOutputDeviceList->setSelectedId(index+1, dontSendNotification);
        
        addAndMakeVisible(midiChannelLabel = new Label(String::empty, L"Channel"));
        midiChannelLabel->setFont(Font(15.0000f, Font::plain));
        midiChannelLabel->setJustificationType(Justification::centredLeft);
        midiChannelLabel->setEnabled(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("midiClick"));
        
        addAndMakeVisible(midiChannel = new TextEditor());
        midiChannel->setText(String(Polytempo_StoredPreferences::getInstance()->getProps().getIntValue("midiChannel")));
        midiChannel->setEnabled(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("midiClick"));
        midiChannel->addListener(this);
    }
    
    ~MidiPreferencesPage()
    {
        deleteAllChildren();
    }
    
    void resized() override
    {
        midiClick->setBounds                (20,  20, getWidth() - 20, 24);
        
        midiDownbeatPitchLabel->setBounds   (20,  50, 100, 24);
        midiDownbeatPitch->setBounds        (120, 50,  80, 20);
        midiDownbeatVelocityLabel->setBounds(20,  70, 100, 24);
        midiDownbeatVelocitySlider->setBounds(120, 70, getWidth() - 140, 24);
        
        midiBeatPitchLabel->setBounds       (20, 100, 100, 24);
        midiBeatPitch->setBounds            (120,100,  80, 20);
        midiBeatVelocityLabel->setBounds    (20, 120, 100, 24);
        midiBeatVelocitySlider->setBounds   (120,120, getWidth() - 140, 24);
        
        midiCuePitchLabel->setBounds        (20, 150, 100, 24);
        midiCuePitch->setBounds             (120,150,  80, 20);
        midiCueVelocityLabel->setBounds     (20, 170, 100, 24);
        midiCueVelocitySlider->setBounds    (120,170, getWidth() - 140, 24);
        
        midiOutputDeviceListLabel->setBounds(20, 250, 100, 24);
        midiOutputDeviceList->setBounds     (120,250, getWidth() - 140, 24);
        midiChannelLabel->setBounds         (20, 280, 100, 24);
        midiChannel->setBounds              (120,280,  80, 20);
    }
    
    /* text editor listener
     --------------------------------------- */
    void textEditorTextChanged(TextEditor& textEditor) override
    {}
    
    void textEditorReturnKeyPressed(TextEditor& textEditor) override
    {
        textEditor.moveKeyboardFocusToSibling(true);
    }
    
    void textEditorEscapeKeyPressed(TextEditor& textEditor) override
    {}
    
    void textEditorFocusLost(TextEditor& textEditor) override
    {
        int value;
        
        if(&textEditor == midiDownbeatPitch || &textEditor == midiBeatPitch)
        {
            value = textEditor.getText().getIntValue();
            value = value > 127 ? 127 : value < 0 ? 0 : value;
            textEditor.setText(String(value));
        }
        else if(&textEditor == midiChannel)
        {
            value = textEditor.getText().getIntValue();
            value = value > 16 ? 16 : value < 1 ? 1 : value;
            textEditor.setText(String(value));
        }
        
        if(&textEditor == midiDownbeatPitch)
        {
            Polytempo_StoredPreferences::getInstance()->getProps().setValue("midiDownbeatPitch", value);
            Polytempo_MidiClick::getInstance()->setDownbeatPitch(value);
        }
        else if(&textEditor == midiBeatPitch)
        {
            Polytempo_StoredPreferences::getInstance()->getProps().setValue("midiBeatPitch", value);
            Polytempo_MidiClick::getInstance()->setBeatPitch(value);
        }
        else if(&textEditor == midiChannel)
        {
            Polytempo_StoredPreferences::getInstance()->getProps().setValue("midiChannel", value);
            Polytempo_MidiClick::getInstance()->setChannel(value);
        }
    }
    
    
    /* slider listener
     --------------------------------------- */
    void sliderValueChanged(Slider* slider) override
    {
        if(slider == midiDownbeatVelocitySlider)
        {
            Polytempo_StoredPreferences::getInstance()->getProps().setValue("midiDownbeatVelocity", slider->getValue());
            Polytempo_MidiClick::getInstance()->setDownbeatVelocity((int)slider->getValue());
        }
        else if(slider == midiBeatVelocitySlider)
        {
            Polytempo_StoredPreferences::getInstance()->getProps().setValue("midiBeatVelocity", slider->getValue());
            Polytempo_MidiClick::getInstance()->setBeatVelocity(slider->getValue());
        }
    }
    
    /* button listener
     --------------------------------------- */
    void buttonClicked(Button* button) override
    {
        if(button == midiClick)
        {
            Polytempo_StoredPreferences::getInstance()->getProps().setValue("midiClick", button->getToggleState());
            
            midiDownbeatPitchLabel->setEnabled(button->getToggleState());
            midiDownbeatPitch->setEnabled(button->getToggleState());
            midiDownbeatVelocityLabel->setEnabled(button->getToggleState());
            midiDownbeatVelocitySlider->setEnabled(button->getToggleState());
            
            midiBeatPitchLabel->setEnabled(button->getToggleState());
            midiBeatPitch->setEnabled(button->getToggleState());
            midiBeatVelocityLabel->setEnabled(button->getToggleState());
            midiBeatVelocitySlider->setEnabled(button->getToggleState());
            
            // cue pitch and volume ...
            
            midiOutputDeviceListLabel->setEnabled(button->getToggleState());
            midiOutputDeviceList->setEnabled(button->getToggleState());
            midiChannelLabel->setEnabled(button->getToggleState());
            midiChannel->setEnabled(button->getToggleState());
        }
    }
    
    void buttonStateChanged(Button* button) override
    {}
    
    /* combo box listener
     --------------------------------------- */
    
    void comboBoxChanged(ComboBox* box) override
    {
        if(box == midiOutputDeviceList)
        {
            int index = midiOutputDeviceList->getSelectedItemIndex();
            Polytempo_MidiClick::getInstance()->setOutputDeviceIndex(index);
            
            juce::StringArray midiDevices = MidiOutput::getDevices();
            Polytempo_StoredPreferences::getInstance()->getProps().setValue("midiOutputDevice", midiDevices[index]);
        }
    }
    
    
    /* change listener
     --------------------------------------- */
    
    void changeListenerCallback(ChangeBroadcaster *source) override
    {}
    
};


//==============================================================================
static const char* generalPreferencesPage = "General";
static const char* visualPreferencesPage = "Visual";
static const char* audioPreferencesPage = "Audio";
static const char* midiPreferencesPage = "Midi";


Polytempo_NetworkPreferencesPanel::Polytempo_NetworkPreferencesPanel()
{
    String preferencePage = Polytempo_StoredPreferences::getInstance()->getProps().getValue("settingsPage");

    addSettingsPage(generalPreferencesPage, 0, 0);
    addSettingsPage(visualPreferencesPage, 0, 0);
    addSettingsPage(audioPreferencesPage, 0, 0);
    addSettingsPage(midiPreferencesPage, 0, 0);
    
    if(preferencePage == String::empty) preferencePage = generalPreferencesPage;
    setCurrentPage(preferencePage);
}

Polytempo_NetworkPreferencesPanel::~Polytempo_NetworkPreferencesPanel()
{
    Polytempo_StoredPreferences::getInstance()->flush();
}


Component* Polytempo_NetworkPreferencesPanel::createComponentForPage(const String& pageName)
{
    Polytempo_StoredPreferences::getInstance()->getProps().setValue("settingsPage", pageName);

    if(pageName == generalPreferencesPage)
        return new GeneralPreferencesPage();
    else if(pageName == visualPreferencesPage)
        return new VisualPreferencesPage();
    else if(pageName == audioPreferencesPage)
        return new AudioPreferencesPage();
    else if(pageName == midiPreferencesPage)
        return new MidiPreferencesPage();

    return new Component();
}

void Polytempo_NetworkPreferencesPanel::show()
{
    Polytempo_NetworkPreferencesPanel *p = new Polytempo_NetworkPreferencesPanel;
    p->setSize(500, 690);

    DialogWindow::LaunchOptions options;
    options.content.setOwned(p);
    options.dialogTitle                   = "Polytempo Network Preferences";
    options.dialogBackgroundColour        = Colours::white;
    //options.componentToCentreAround       = ;
    options.escapeKeyTriggersCloseButton  = true;
    options.useNativeTitleBar             = true;
    options.resizable                     = false;
    
    options.launchAsync();
}

