#include "Polytempo_GraphicExportSettingsComponent.h"
#include "Polytempo_GraphicExportView.h"

Polytempo_GraphicExportSettingsComponent::Polytempo_GraphicExportSettingsComponent()
{
    exportPagesButton = new Polytempo_Button("Export Pages", Polytempo_Button::buttonType_normal);
    addAndMakeVisible(exportPagesButton);
    exportPagesButton->addListener(this);

    timeFactorTextbox = new Polytempo_Textbox(String());
    addAndMakeVisible(timeFactorTextbox);
    timeFactorTextbox->setFont(Font (24.0f, Font::bold));
    timeFactorTextbox->setInputRestrictions(0, "0123456789.");
    timeFactorTextbox->setText("1.0", dontSendNotification);
    timeFactorTextbox->addListener(this);
    
    systemHeightTextbox = new Polytempo_Textbox(String());
    addAndMakeVisible(systemHeightTextbox);
    systemHeightTextbox->setFont(Font (24.0f, Font::bold));
    systemHeightTextbox->setInputRestrictions(0, "0123456789");
    systemHeightTextbox->setText("700", dontSendNotification);
    systemHeightTextbox->addListener(this);

    addAndMakeVisible(landscapeButton = new ToggleButton("Landscape"));
    landscapeButton->addListener(this);
}

Polytempo_GraphicExportSettingsComponent::~Polytempo_GraphicExportSettingsComponent()
{
    deleteAllChildren();
}

void Polytempo_GraphicExportSettingsComponent::setGraphicExportView(Component* view)
{
    graphicExportView = view;

    ((Polytempo_GraphicExportView *)graphicExportView)->setTimeFactor(timeFactorTextbox->getText().getFloatValue());
    ((Polytempo_GraphicExportView *)graphicExportView)->setSystemHeight(systemHeightTextbox->getText().getIntValue());
}

void Polytempo_GraphicExportSettingsComponent::paint (Graphics& g)
{
    g.fillAll(Colour(245,245,245));
    
    g.setColour(Colour(170,170,170));
    g.drawHorizontalLine(0, 0.0f, float(getWidth()));

    g.drawText("Time Factor", 5, int(getHeight() * 0.25), 100, 20, Justification::left);
    g.drawText("System Height", 195, int(getHeight() * 0.25), 100, 20, Justification::left);
}

void Polytempo_GraphicExportSettingsComponent::resized()
{
    exportPagesButton->setBounds(getWidth() - 105, int((getHeight() - 22) * 0.5), 100, 22);

    timeFactorTextbox->setBounds(85, int((getHeight() - 22) * 0.5), 50, 22);
    systemHeightTextbox->setBounds(285, int((getHeight() - 22) * 0.5), 70, 22);
    
    landscapeButton->setBounds(420, int((getHeight() - 22) * 0.5), 150, 22);
}

void Polytempo_GraphicExportSettingsComponent::buttonClicked(Button* button)
{
    if(button == exportPagesButton)
        ((Polytempo_GraphicExportView *)graphicExportView)->exportPages();
    else if(button == landscapeButton)
        ((Polytempo_GraphicExportView *)graphicExportView)->setLandscape(button->getToggleState());
}

void Polytempo_GraphicExportSettingsComponent::editorShown (Label* label, TextEditor&)
{
    if(label == timeFactorTextbox)        timeFactorString   = label->getText();
    else if(label == systemHeightTextbox) systemHeightString = label->getText();
}

void Polytempo_GraphicExportSettingsComponent::labelTextChanged (Label* label)
{
    if(label == timeFactorTextbox)
    {
        float num = label->getText().getFloatValue();
        if(num <= 0.0)
            label->setText(timeFactorString, dontSendNotification);
        else
            ((Polytempo_GraphicExportView *)graphicExportView)->setTimeFactor(num);
    }
    else if(label == systemHeightTextbox)
    {
        int num = label->getText().getIntValue();
        ((Polytempo_GraphicExportView *)graphicExportView)->setSystemHeight(num);
    }
}
