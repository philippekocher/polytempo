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

#include "Polytempo_ComposerPreferencesPanel.h"
#include "Polytempo_StoredPreferences.h"
#include "../Misc/Rational.h"
#include "../Application/PolytempoComposer/Polytempo_ComposerApplication.h"


//------------------------------------------------------------------------------
#pragma mark -
#pragma mark General Preferences Page

class GeneralPreferencesPage : public Component, Button::Listener, TextEditor::Listener
{
    Label      *tempoMeasurementLabel;
    Label      *tempoMeasurementUnitLabel;
    TextEditor *tempoMeasurementUnit;
    Label      *tempoMeasurementTimeLabel;
    TextEditor *tempoMeasurementTime;
    
    Label      *defaultBeatPatternLabel;
    Label      *defaultBeatPatternMetreLabel;
    TextEditor *defaultBeatPatternMetre;
    Label      *defaultBeatPatternRepeatsLabel;
    TextEditor *defaultBeatPatternRepeats;

public:
    GeneralPreferencesPage()
    {
        // tempo measurement
        
        String tempoMeasurement = Polytempo_StoredPreferences::getInstance()->getProps().getValue("tempoMeasurement");
        
        StringArray tempoMeasurementTokens;
        tempoMeasurementTokens.addTokens(tempoMeasurement, false);

        addAndMakeVisible(tempoMeasurementLabel = new Label(String::empty, L"Tempo Measurement"));
        tempoMeasurementLabel->setFont(Font(15.0000f, Font::plain));
        tempoMeasurementLabel->setJustificationType(Justification::centredLeft);
        tempoMeasurementLabel->setEditable(false, false, false);
        
        addAndMakeVisible(tempoMeasurementUnitLabel = new Label(String::empty, L"Unit:"));
        tempoMeasurementUnitLabel->setFont(Font(12.0000f, Font::plain));
        tempoMeasurementUnitLabel->setJustificationType(Justification::centredLeft);
        tempoMeasurementUnitLabel->setEditable(false, false, false);
        
        addAndMakeVisible(tempoMeasurementUnit = new TextEditor());
        tempoMeasurementUnit->setText(tempoMeasurementTokens[0]);
        tempoMeasurementUnit->setBorder(BorderSize<int>(1));
        tempoMeasurementUnit->setColour(TextEditor::outlineColourId, Colours::grey);
        tempoMeasurementUnit->setInputRestrictions(0, "0123456789/");
        tempoMeasurementUnit->addListener(this);
        
        addAndMakeVisible(tempoMeasurementTimeLabel = new Label(String::empty, L"Time:"));
        tempoMeasurementTimeLabel->setFont(Font(12.0000f, Font::plain));
        tempoMeasurementTimeLabel->setJustificationType(Justification::centredLeft);
        tempoMeasurementTimeLabel->setEditable(false, false, false);
        
        addAndMakeVisible(tempoMeasurementTime = new TextEditor());
        tempoMeasurementTime->setText(tempoMeasurementTokens[1]);
        tempoMeasurementTime->setBorder(BorderSize<int>(1));
        tempoMeasurementTime->setColour(TextEditor::outlineColourId, Colours::grey);
        tempoMeasurementTime->setInputRestrictions(0, "0123456789.");
        tempoMeasurementTime->addListener(this);
 
        
        // default beat pattern
        
        String defaultBeatPattern = Polytempo_StoredPreferences::getInstance()->getProps().getValue("defaultBeatPattern");
        
        StringArray beatPatternTokens;
        beatPatternTokens.addTokens(defaultBeatPattern, false);

        addAndMakeVisible(defaultBeatPatternLabel = new Label(String::empty, L"Default Beat Pattern"));
        defaultBeatPatternLabel->setFont(Font(15.0000f, Font::plain));
        defaultBeatPatternLabel->setJustificationType(Justification::centredLeft);
        defaultBeatPatternLabel->setEditable(false, false, false);
        
        addAndMakeVisible(defaultBeatPatternMetreLabel = new Label(String::empty, L"Metre:"));
        defaultBeatPatternMetreLabel->setFont(Font(12.0000f, Font::plain));
        defaultBeatPatternMetreLabel->setJustificationType(Justification::centredLeft);
        defaultBeatPatternMetreLabel->setEditable(false, false, false);
        
        addAndMakeVisible(defaultBeatPatternMetre = new TextEditor());
        defaultBeatPatternMetre->setText(beatPatternTokens[0]);
        defaultBeatPatternMetre->setBorder(BorderSize<int>(1));
        defaultBeatPatternMetre->setColour(TextEditor::outlineColourId, Colours::grey);
        defaultBeatPatternMetre->setInputRestrictions(0, "0123456789/");
        defaultBeatPatternMetre->addListener(this);
        
        addAndMakeVisible(defaultBeatPatternRepeatsLabel = new Label(String::empty, L"Repeats:"));
        defaultBeatPatternRepeatsLabel->setFont(Font(12.0000f, Font::plain));
        defaultBeatPatternRepeatsLabel->setJustificationType(Justification::centredLeft);
        defaultBeatPatternRepeatsLabel->setEditable(false, false, false);
        
        addAndMakeVisible(defaultBeatPatternRepeats = new TextEditor());
        defaultBeatPatternRepeats->setText(beatPatternTokens[1]);
        defaultBeatPatternRepeats->setBorder(BorderSize<int>(1));
        defaultBeatPatternRepeats->setColour(TextEditor::outlineColourId, Colours::grey);
        defaultBeatPatternRepeats->setInputRestrictions(0, "0123456789");
        defaultBeatPatternRepeats->addListener(this);

    }
    
    ~GeneralPreferencesPage()
    {
        deleteAllChildren();
    }
    
    void resized()
    {
        tempoMeasurementLabel->setBounds    (18,  35, 200, 24);
        tempoMeasurementUnitLabel->setBounds(20,  70, 50, 24);
        tempoMeasurementUnit->setBounds     (70,  70, 40, 24);
        tempoMeasurementTimeLabel->setBounds(120, 70, 50, 24);
        tempoMeasurementTime->setBounds     (170, 70, 40, 24);

        defaultBeatPatternLabel->setBounds       (18,  135, 200, 24);
        defaultBeatPatternMetreLabel->setBounds  (20,  170, 50, 24);
        defaultBeatPatternMetre->setBounds       (70,  170, 40, 24);
        defaultBeatPatternRepeatsLabel->setBounds(120, 170, 50, 24);
        defaultBeatPatternRepeats->setBounds     (170, 170, 40, 24);
}
    
    /* text editor & button listener
     --------------------------------------- */
    void textEditorTextChanged(TextEditor& editor)
    {}
    
    void textEditorReturnKeyPressed(TextEditor& editor)
    {
        Component::unfocusAllComponents();
    }
    
    void textEditorEscapeKeyPressed(TextEditor&)
    {
        Component::unfocusAllComponents();
    }
    
    void textEditorFocusLost (TextEditor& editor)
    {
        if(&editor == tempoMeasurementUnit || &editor == tempoMeasurementTime)
        {
            Rational unit = Rational(tempoMeasurementUnit->getText());
            if(unit == 0) unit = Rational("1/1");
            tempoMeasurementUnit->setText(unit.toString());
            
            String time = tempoMeasurementTime->getText();
            if(time == String::empty) time = "1";
            if(time.getFloatValue() == 0.0f) time = "1";
            tempoMeasurementTime->setText(time);
            
            Polytempo_StoredPreferences::getInstance()->getProps().setValue("tempoMeasurement", unit.toString()+" "+time);

            // everything needs to be repainted when the tempo measurement changes
            Polytempo_ComposerApplication* const app = dynamic_cast<Polytempo_ComposerApplication*>(JUCEApplication::getInstance());
            app->getMainView().repaint();
        }
        else if(&editor == defaultBeatPatternMetre || &editor == defaultBeatPatternRepeats)
        {
            Rational metre = Rational(defaultBeatPatternMetre->getText());
            if(metre == 0) metre = Rational(1);
            defaultBeatPatternMetre->setText(metre.toString());
            
            String repeats = defaultBeatPatternRepeats->getText();
            if(repeats == String::empty) repeats = "1";
            if(repeats == "0") repeats = "1";
            defaultBeatPatternRepeats->setText(repeats);

            Polytempo_StoredPreferences::getInstance()->getProps().setValue("defaultBeatPattern", metre.toString()+" "+repeats);
        }
    }
    
    void buttonClicked(Button* button)
    {}
    
    void buttonStateChanged(Button&)
    {}
};

//------------------------------------------------------------------------------
#pragma mark -
#pragma mark Preferences Panel

static const char* generalPreferencesPage = "General";

Polytempo_ComposerPreferencesPanel::Polytempo_ComposerPreferencesPanel()
{
    String preferencePage = Polytempo_StoredPreferences::getInstance()->getProps().getValue("settingsPage");
    
    addSettingsPage(generalPreferencesPage, 0, 0);
    
    if(preferencePage == String::empty) preferencePage = generalPreferencesPage;
    setCurrentPage(preferencePage);
}

Polytempo_ComposerPreferencesPanel::~Polytempo_ComposerPreferencesPanel()
{
    Polytempo_StoredPreferences::getInstance()->flush();
}

Component* Polytempo_ComposerPreferencesPanel::createComponentForPage(const String& pageName)
{
    Polytempo_StoredPreferences::getInstance()->getProps().setValue("settingsPage", pageName);
    
    return new GeneralPreferencesPage();
}

void Polytempo_ComposerPreferencesPanel::show()
{
    Polytempo_ComposerPreferencesPanel *p = new Polytempo_ComposerPreferencesPanel;
    p->setSize(500, 690);
    
    DialogWindow::LaunchOptions options;
    options.content.setOwned(p);
    options.dialogTitle                   = "Polytempo Composer Preferences";
    options.dialogBackgroundColour        = Colours::white;
    //options.componentToCentreAround       = ;
    options.escapeKeyTriggersCloseButton  = true;
    options.useNativeTitleBar             = true;
    options.resizable                     = false;
    
    options.launchAsync();
}
