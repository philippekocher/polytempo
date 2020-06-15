#include "Polytempo_AuxiliaryView.h"
#include "../../Scheduler/Polytempo_ScoreScheduler.h"
#include "../../Scheduler/Polytempo_EventDispatcher.h"
#include "../../Preferences/Polytempo_StoredPreferences.h"
#include "../../Misc/Polytempo_Textbox.h"
#include "../../Network/Polytempo_TimeProvider.h"
#include "Polytempo_GraphicsPalette.h"

Polytempo_AuxiliaryView::Polytempo_AuxiliaryView()
{
    addAndMakeVisible(imageBackwards = new Polytempo_Button("<<", Polytempo_Button::buttonType_black));
    imageBackwards->setConnectedEdges(Button::ConnectedOnTop | Button::ConnectedOnRight);
    imageBackwards->addListener(this);

    addAndMakeVisible(markerBackwards = new Polytempo_Button("<", Polytempo_Button::buttonType_black));
    markerBackwards->setConnectedEdges(Button::ConnectedOnTop | Button::ConnectedOnRight | Button::ConnectedOnLeft);
    markerBackwards->addListener(this);

    addAndMakeVisible(markerForwards = new Polytempo_Button(">", Polytempo_Button::buttonType_black));
    markerForwards->setConnectedEdges(Button::ConnectedOnTop | Button::ConnectedOnRight | Button::ConnectedOnLeft);
    markerForwards->addListener(this);

    addAndMakeVisible(imageForwards = new Polytempo_Button(">>", Polytempo_Button::buttonType_black));
    imageForwards->setConnectedEdges(Button::ConnectedOnTop | Button::ConnectedOnLeft);
    imageForwards->addListener(this);

    addAndMakeVisible(startStop = new Polytempo_Button("Start / Stop", Polytempo_Button::buttonType_black));
    startStop->setConnectedEdges(Button::ConnectedOnBottom);
    startStop->addListener(this);

    addAndMakeVisible(returnToLocator = new Polytempo_Button("RTL", Polytempo_Button::buttonType_black));
    returnToLocator->setConnectedEdges(Button::ConnectedOnTop | Button::ConnectedOnRight);
    returnToLocator->addListener(this);

    addAndMakeVisible(returnToBeginning = new Polytempo_Button("RTB", Polytempo_Button::buttonType_black));
    returnToBeginning->setConnectedEdges(Button::ConnectedOnTop | Button::ConnectedOnLeft);
    returnToBeginning->addListener(this);

    addAndMakeVisible(markerTextbox = new Polytempo_Textbox("Marker", Polytempo_Textbox::textboxType_black));
    markerTextbox->setFont(Font(56.0f, Font::plain));
    markerTextbox->addListener(this);

    addAndMakeVisible(timeTextbox = new Polytempo_Textbox("Time", Polytempo_Textbox::textboxType_black));
    timeTextbox->setFont(Font(24.0f, Font::plain));
    timeTextbox->addListener(this);

    addAndMakeVisible(tempoFactorTextbox = new Polytempo_Textbox("Tempo x", Polytempo_Textbox::textboxType_black));
    tempoFactorTextbox->setFont(Font(24.0000f, Font::plain));
    tempoFactorTextbox->addListener(this);
    tempoFactorTextbox->setText("1.0", dontSendNotification);

    addAndMakeVisible(annotationView = new Polytempo_AnnotationView());

    addAndMakeVisible(timeSyncControl = new Polytempo_TimeSyncControl());
    Polytempo_TimeProvider::getInstance()->registerUserInterface(timeSyncControl);

    addAndMakeVisible(networkInfoView = new Polytempo_NetworkInfoView());
    networkInfoView->setColour(Label::backgroundColourId, Colours::lightblue);
}

Polytempo_AuxiliaryView::~Polytempo_AuxiliaryView()
{
    Polytempo_GraphicsPalette::deleteInstance();

    deleteAllChildren();
}

void Polytempo_AuxiliaryView::paint(Graphics& g)
{
    g.fillAll(Colours::white); // clear the background

    networkInfoView->repaint();

    g.setColour(Colours::black);
    g.drawHorizontalLine(separator1Position, 0.0f, (float)getWidth());
    g.drawHorizontalLine(separator2Position, 0.0f, (float)getWidth());
    g.setColour(Colours::grey);
    g.drawVerticalLine(0, 0.0f, (float)getHeight());
}

void Polytempo_AuxiliaryView::resized()
{
    int yPosition = 10;

    int buttonWidth = (int)((getWidth() - 20) * 0.5f);
    int widthCorrection = getWidth() - 20 - buttonWidth * 2;
    startStop->setBounds(10, yPosition, getWidth() - 20, 22);
    returnToLocator->setBounds(10, yPosition + 22, buttonWidth, 22);
    returnToBeginning->setBounds(10 + buttonWidth, yPosition + 22, buttonWidth + widthCorrection, 22);
    yPosition += 65;

    markerTextbox->setBounds(10, yPosition, getWidth() - 20, 66);
    yPosition += 66;

    buttonWidth = (int)(getWidth() * 0.25f - 5.0f);
    widthCorrection = getWidth() - 20 - buttonWidth * 4;
    imageBackwards->setBounds(10, yPosition, buttonWidth, 20);
    markerBackwards->setBounds(10 + buttonWidth * 1, yPosition, buttonWidth, 20);
    markerForwards->setBounds(10 + buttonWidth * 2, yPosition, buttonWidth, 20);
    imageForwards->setBounds(10 + buttonWidth * 3, yPosition, buttonWidth + widthCorrection, 20);
    yPosition += 43;

    timeTextbox->setBounds(10, yPosition, getWidth() - 20, 34);
    yPosition += 55;

    tempoFactorTextbox->setBounds(10, yPosition, getWidth() - 20, 34);
    yPosition += 50;

    separator1Position = yPosition;
    yPosition += 10;

    annotationView->setBounds(10, yPosition, getWidth() - 20, annotationView->getHeight());
    yPosition += annotationView->getHeight() + 5;

    separator2Position = yPosition;
    yPosition += 5;

    timeSyncControl->setBounds(10, yPosition, getWidth() - 20, 50);
    yPosition += 45;

    networkInfoView->setBounds(10, yPosition, getWidth() - 20, getHeight() - yPosition);

    repaint();
}

void Polytempo_AuxiliaryView::eventNotification(Polytempo_Event* event)
{
    if (!Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("showAuxiliaryView")) return;

    if (event->getType() == eventType_Marker)
    {
        markerTextbox->setText(event->getProperty("value"), dontSendNotification);
    }
    else if (event->getType() == eventType_TempoFactor)
    {
        tempoFactorTextbox->setFloat(event->getProperty("value"), dontSendNotification);
    }
    else if (event->getType() == eventType_Tick)
    {
        // update the time with every tick
        timeTextbox->setText(Polytempo_Textbox::timeToString(event->getValue()), dontSendNotification);
    }
    else if (event->getType() == eventType_DeleteAll)
    {
        markerTextbox->reset();
        tempoFactorTextbox->reset();
    }
}

void Polytempo_AuxiliaryView::editorShown(Label* label, TextEditor&)
{
    if (label == markerTextbox) markerString = label->getText();
    else if (label == tempoFactorTextbox) tempoFactor = label->getText();
}

void Polytempo_AuxiliaryView::labelTextChanged(Label* label)
{
    if (label == markerTextbox)
    {
        DBG(label->getText());
        if (!Polytempo_ScoreScheduler::getInstance()->gotoMarker(label->getText()))
            label->setText(markerString, dontSendNotification);
    }
    else if (label == timeTextbox)
    {
        Polytempo_ScoreScheduler::getInstance()->storeLocator(int(Polytempo_Textbox::stringToTime(label->getText()) * 1000.0f));
        Polytempo_EventDispatcher::getInstance()->broadcastEvent(Polytempo_Event::makeEvent(eventType_GotoTime, Polytempo_Textbox::stringToTime(label->getText())));
    }
    else if (label == tempoFactorTextbox)
    {
        float num = (label->getText()).getFloatValue();
        if (num <= 0.0)
            label->setText(tempoFactor, dontSendNotification);
        else
            Polytempo_EventDispatcher::getInstance()->broadcastEvent(Polytempo_Event::makeEvent(eventType_TempoFactor, num));
    }
}

// Button listener
void Polytempo_AuxiliaryView::buttonClicked(Button* button)
{
    if (button == markerBackwards)
        Polytempo_ScoreScheduler::getInstance()->skipToEvent(eventType_Marker, true);
    if (button == markerForwards)
        Polytempo_ScoreScheduler::getInstance()->skipToEvent(eventType_Marker, false);
    if (button == imageBackwards)
        Polytempo_ScoreScheduler::getInstance()->skipToEvent(eventType_Image, true);
    if (button == imageForwards)
        Polytempo_ScoreScheduler::getInstance()->skipToEvent(eventType_Image, false);
    if (button == startStop)
        Polytempo_ScoreScheduler::getInstance()->startStop();
    if (button == returnToLocator)
        Polytempo_ScoreScheduler::getInstance()->returnToLocator();
    if (button == returnToBeginning)
        Polytempo_ScoreScheduler::getInstance()->returnToBeginning();
}

void Polytempo_AuxiliaryView::buttonStateChanged(Button*)
{
}
