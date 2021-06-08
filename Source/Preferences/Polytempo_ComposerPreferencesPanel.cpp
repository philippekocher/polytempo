#include "Polytempo_ComposerPreferencesPanel.h"
#include "Polytempo_StoredPreferences.h"
#include "../Audio+Midi/Polytempo_MidiClick.h"
#include "../Misc/Rational.h"
#include "../Application/PolytempoComposer/Polytempo_ComposerApplication.h"
#include "../Application/Polytempo_LatestVersionChecker.h"


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

    ToggleButton* checkForNewVersionToggle;
    TextButton* checkNowButton;

public:
    GeneralPreferencesPage()
    {
        // tempo measurement
        
        String tempoMeasurement = Polytempo_StoredPreferences::getInstance()->getProps().getValue("tempoMeasurement");
        
        StringArray tempoMeasurementTokens;
        tempoMeasurementTokens.addTokens(tempoMeasurement, false);

        addAndMakeVisible(tempoMeasurementLabel = new Label(String(), L"Tempo Measurement"));
        tempoMeasurementLabel->setFont(Font(15.0000f, Font::plain));
        tempoMeasurementLabel->setJustificationType(Justification::centredLeft);
        tempoMeasurementLabel->setEditable(false, false, false);
        
        addAndMakeVisible(tempoMeasurementUnitLabel = new Label(String(), L"Unit:"));
        tempoMeasurementUnitLabel->setFont(Font(12.0000f, Font::plain));
        tempoMeasurementUnitLabel->setJustificationType(Justification::centredLeft);
        tempoMeasurementUnitLabel->setEditable(false, false, false);
        
        addAndMakeVisible(tempoMeasurementUnit = new TextEditor());
        tempoMeasurementUnit->setText(tempoMeasurementTokens[0]);
        tempoMeasurementUnit->setBorder(BorderSize<int>(1));
        tempoMeasurementUnit->setColour(TextEditor::outlineColourId, Colours::grey);
        tempoMeasurementUnit->setInputRestrictions(0, "0123456789/");
        tempoMeasurementUnit->addListener(this);
        
        addAndMakeVisible(tempoMeasurementTimeLabel = new Label(String(), L"Time:"));
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

        addAndMakeVisible(defaultBeatPatternLabel = new Label(String(), L"Default Beat Pattern"));
        defaultBeatPatternLabel->setFont(Font(15.0000f, Font::plain));
        defaultBeatPatternLabel->setJustificationType(Justification::centredLeft);
        defaultBeatPatternLabel->setEditable(false, false, false);
        
        addAndMakeVisible(defaultBeatPatternMetreLabel = new Label(String(), L"Metre:"));
        defaultBeatPatternMetreLabel->setFont(Font(12.0000f, Font::plain));
        defaultBeatPatternMetreLabel->setJustificationType(Justification::centredLeft);
        defaultBeatPatternMetreLabel->setEditable(false, false, false);
        
        addAndMakeVisible(defaultBeatPatternMetre = new TextEditor());
        defaultBeatPatternMetre->setText(beatPatternTokens[0]);
        defaultBeatPatternMetre->setBorder(BorderSize<int>(1));
        defaultBeatPatternMetre->setColour(TextEditor::outlineColourId, Colours::grey);
        defaultBeatPatternMetre->setInputRestrictions(0, "0123456789/");
        defaultBeatPatternMetre->addListener(this);
        
        addAndMakeVisible(defaultBeatPatternRepeatsLabel = new Label(String(), L"Repeats:"));
        defaultBeatPatternRepeatsLabel->setFont(Font(12.0000f, Font::plain));
        defaultBeatPatternRepeatsLabel->setJustificationType(Justification::centredLeft);
        defaultBeatPatternRepeatsLabel->setEditable(false, false, false);
        
        addAndMakeVisible(defaultBeatPatternRepeats = new TextEditor());
        defaultBeatPatternRepeats->setText(beatPatternTokens[1]);
        defaultBeatPatternRepeats->setBorder(BorderSize<int>(1));
        defaultBeatPatternRepeats->setColour(TextEditor::outlineColourId, Colours::grey);
        defaultBeatPatternRepeats->setInputRestrictions(0, "0123456789");
        defaultBeatPatternRepeats->addListener(this);


        // check for new version
        
        addAndMakeVisible(checkForNewVersionToggle = new ToggleButton(String()));
        checkForNewVersionToggle->setButtonText(L"Automatically check for new versions");
        checkForNewVersionToggle->setToggleState(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("checkForNewVersion"), dontSendNotification);
        checkForNewVersionToggle->addListener(this);
        
        addAndMakeVisible(checkNowButton = new TextButton("Check now"));
        checkNowButton->addListener(this);
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

        checkForNewVersionToggle->setBounds(20, 280, 260, 24);
        checkNowButton->setBounds          (290, 280, 100, 24);
}
    
    /* text editor & button listener
     --------------------------------------- */
    void textEditorTextChanged(TextEditor&)
    {}
    
    void textEditorReturnKeyPressed(TextEditor&)
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
            if(time == String()) time = "1";
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
            if(repeats == String()) repeats = "1";
            if(repeats == "0") repeats = "1";
            defaultBeatPatternRepeats->setText(repeats);

            Polytempo_StoredPreferences::getInstance()->getProps().setValue("defaultBeatPattern", metre.toString()+" "+repeats);
        }
    }
    
    void buttonClicked(Button* button)
    {
        if(button == checkForNewVersionToggle)
        {
            Polytempo_StoredPreferences::getInstance()->getProps().setValue("checkForNewVersion", button->getToggleState());
        }
        else if(button == checkNowButton)
        {
            Polytempo_LatestVersionChecker::getInstance()->checkForNewVersion(true);
        }
    }
    
    void buttonStateChanged(Button&)
    {}
};

//------------------------------------------------------------------------------
#pragma mark -
#pragma mark Midi Preferences Page

class MidiPreferencesPage : public Component,
                            ComboBox::Listener,
                            ChangeListener
{
    Label* midiOutputDeviceListLabel;
    ComboBox* midiOutputDeviceList;

public:
    MidiPreferencesPage()
    {
        addAndMakeVisible(midiOutputDeviceListLabel = new Label(String(), "MIDI Output"));
        midiOutputDeviceListLabel->setFont(Font(15.0000f, Font::plain));
        midiOutputDeviceListLabel->setJustificationType(Justification::centredLeft);

        addAndMakeVisible(midiOutputDeviceList = new ComboBox());
        midiOutputDeviceList->setTextWhenNoChoicesAvailable("No MIDI Outputs Enabled");
        const StringArray midiOutputs(MidiOutput::getDevices());
        midiOutputDeviceList->addItemList(midiOutputs, 1);
        midiOutputDeviceList->addListener(this);

        // find the device stored in the settings
        StringArray midiDevices = MidiOutput::getDevices();
        int index = midiDevices.indexOf(Polytempo_StoredPreferences::getInstance()->getProps().getValue("midiOutputDevice"));
        if (index < 0) index = 0; // otherwise set first device
        midiOutputDeviceList->setSelectedId(index + 1, dontSendNotification);
    }

    ~MidiPreferencesPage()
    {
        deleteAllChildren();
    }

    void resized() override
    {
        midiOutputDeviceListLabel->setBounds(20, 35, 100, 24);
        midiOutputDeviceList->setBounds(120, 35, getWidth() - 140, 24);
    }

    /* combo box listener
     --------------------------------------- */

    void comboBoxChanged(ComboBox* box) override
    {
        if (box == midiOutputDeviceList)
        {
            int index = midiOutputDeviceList->getSelectedItemIndex();
            Polytempo_MidiClick::getInstance()->setOutputDeviceIndex(index);

            StringArray midiDevices = MidiOutput::getDevices();
            Polytempo_StoredPreferences::getInstance()->getProps().setValue("midiOutputDevice", midiDevices[index]);
        }
    }

    /* change listener
     --------------------------------------- */

    void changeListenerCallback(ChangeBroadcaster*) override
    {
    }
};
//------------------------------------------------------------------------------
#pragma mark -
#pragma mark Preferences Panel

static const char* generalPreferencesPage = "General";
static const char* midiPreferencesPage = "Midi";

Polytempo_ComposerPreferencesPanel::Polytempo_ComposerPreferencesPanel()
{
    String preferencePage = Polytempo_StoredPreferences::getInstance()->getProps().getValue("settingsPage");
    
    addSettingsPage(generalPreferencesPage, nullptr, 0);
    addSettingsPage(midiPreferencesPage, nullptr, 0);

    if(preferencePage == String()) preferencePage = generalPreferencesPage;
    setCurrentPage(preferencePage);
}

Polytempo_ComposerPreferencesPanel::~Polytempo_ComposerPreferencesPanel()
{
    Polytempo_StoredPreferences::getInstance()->flush();
}

Component* Polytempo_ComposerPreferencesPanel::createComponentForPage(const String& pageName)
{
    Polytempo_StoredPreferences::getInstance()->getProps().setValue("settingsPage", pageName);
    
    if (pageName == midiPreferencesPage)
        return new MidiPreferencesPage();

    return new GeneralPreferencesPage();
}

void Polytempo_ComposerPreferencesPanel::show()
{
    Polytempo_ComposerPreferencesPanel *p = new Polytempo_ComposerPreferencesPanel;
    p->setSize(500, 420);
    
    DialogWindow::LaunchOptions options;
    options.content.setOwned(p);
    options.dialogTitle                   = "Polytempo Composer Preferences";
    options.dialogBackgroundColour        = Colours::white;
    options.escapeKeyTriggersCloseButton  = true;
    options.useNativeTitleBar             = true;
    options.resizable                     = false;
    
    options.launchAsync();
}
