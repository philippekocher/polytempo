#include "Polytempo_SectionInstancesComponent.h"
#include "../../Application/PolytempoNetwork/Polytempo_NetworkApplication.h"

Polytempo_SectionInstancesComponent::Polytempo_SectionInstancesComponent()
{}

Polytempo_SectionInstancesComponent::~Polytempo_SectionInstancesComponent()
{
    deleteAllChildren();
}

void Polytempo_SectionInstancesComponent::paint(Graphics& g)
{
    g.setColour(Colours::grey);
    for(int i=0;i<imageEvents.size();i++)
        g.drawHorizontalLine(i*120-4, 0.0f, float(getWidth()));
}

void Polytempo_SectionInstancesComponent::resized()
{
    for(int i=0;i<markerTextboxes.size();i++)
    {
        regionTextboxes[i]->setBounds(10, i * 120 + 20, int((getWidth() - 20) * 0.3), 28);
        deleteButtons[i]->setBounds(10 + int((getWidth() - 20) * 0.75), i * 120 + 20, int((getWidth() - 20) * 0.25), 18);
        markerTextboxes[i]->setBounds(10, i * 120 + 65, int((getWidth() - 20) * 0.3), 28);
        timeTextboxes[i]->setBounds(10 + int((getWidth() - 20) * 0.35), i * 120 + 65, int((getWidth() - 20) * 0.65), 28);
    }
}

void Polytempo_SectionInstancesComponent::setModel(Polytempo_PageEditorView *model)
{
    pageEditorView = model;
}

void Polytempo_SectionInstancesComponent::setImageEvents(Array< Polytempo_Event* >& imageEvents_, var& sectionID)
{
    imageEvents.clear();
    
    Polytempo_NetworkApplication* const app = dynamic_cast<Polytempo_NetworkApplication*>(JUCEApplication::getInstance());
    score = app->getScore();
    
    for(Polytempo_Event *event : imageEvents_)
    {
        if(event->getProperty(eventPropertyString_SectionID) == sectionID)
            imageEvents.add(event);
    }
    
    // adjust number of textboxes
    addTextboxes(imageEvents.size() - markerTextboxes.size());
    
    // hide unused textboxes
    for(int i=imageEvents.size();i<markerTextboxes.size();i++)
    {
        markerTextboxes[i]->setVisible(false);
        timeTextboxes[i]->setVisible(false);
        regionTextboxes[i]->setVisible(false);
        deleteButtons[i]->setVisible(false);
    }
    
    // adjust height
    setBounds(0,0,getWidth(),imageEvents.size()*120);
    resized();
    
    for(int i=0;i<imageEvents.size();i++)
    {
        markerTextboxes[i]->setVisible(true);
        timeTextboxes[i]->setVisible(true);
        regionTextboxes[i]->setVisible(true);
        deleteButtons[i]->setVisible(true);

        String marker;
        if(score->getMarkerForTime(imageEvents[i]->getTime(), &marker))
            markerTextboxes[i]->setText(marker, dontSendNotification);
        else
            markerTextboxes[i]->reset();
        timeTextboxes[i]->setText(Polytempo_Textbox::timeToString(imageEvents[i]->getTime() * 0.001f), dontSendNotification);
        regionTextboxes[i]->setText(imageEvents[i]->getProperty(eventPropertyString_RegionID).toString(),dontSendNotification);
    }
}

void Polytempo_SectionInstancesComponent::editorShown(Label*, TextEditor&)
{}

void Polytempo_SectionInstancesComponent::labelTextChanged(Label* label)
{
    if(label->getName() == "Marker")
    {
        for(int i=0;i<markerTextboxes.size();i++)
        {
            if(markerTextboxes[i] == label)
            {
                String marker = markerTextboxes[i]->getTextValue().toString();
                int time;

                if(score->getTimeForMarker(marker, &time))
                {
                    imageEvents[i]->setTime(time);
                    score->sortSection();
                    score->setDirty();
                    pageEditorView->update();
                }
                else
                {
                    markerTextboxes[i]->reset();
                }
            }
        }
    }
    else if(label->getName() == "Time")
    {
        for(int i=0;i<timeTextboxes.size();i++)
        {
            if(timeTextboxes[i] == label)
            {
                int time = int(Polytempo_Textbox::stringToTime(timeTextboxes[i]->getText()) * 1000.0f);

                timeTextboxes[i]->setText(Polytempo_Textbox::timeToString(time * 0.001f), dontSendNotification);

                imageEvents[i]->setTime(time);
                score->sortSection();
                score->setDirty();
                pageEditorView->update();
            }
        }
    }
    else if(label->getName() == "Region")
    {
        for(int i=0;i<regionTextboxes.size();i++)
        {
            if(regionTextboxes[i] == label)
            {
                int num = (label->getText()).getIntValue();
                // TODO: check if region exists

                imageEvents[i]->setProperty(eventPropertyString_RegionID, var(num));
                score->setDirty();
                pageEditorView->update();
            }
        }
    }
}

void Polytempo_SectionInstancesComponent::buttonClicked(Button* button)
{
    for(int i=0;i<deleteButtons.size();i++)
    {
        if(deleteButtons[i] == button)
        {
            score->removeEvent(imageEvents[i]);
            score->setDirty();
            pageEditorView->refresh();
        }
    }
}

void Polytempo_SectionInstancesComponent::addTextboxes(int num)
{
    while(num-- > 0)
    {
        Polytempo_Textbox *markerTextbox = new Polytempo_Textbox("Marker");
        markerTextbox->setFont(Font (18.0f, Font::plain));
        markerTextbox->addListener(this);
        markerTextbox->setName("Marker");
        addChildComponent(markerTextbox);
        markerTextboxes.add(markerTextbox);
        
        Polytempo_Textbox *timeTextbox = new Polytempo_Textbox("Time");
        timeTextbox->setFont(Font (18.0f, Font::plain));
        timeTextbox->addListener(this);
        timeTextbox->setName("Time");
        addChildComponent(timeTextbox);
        timeTextboxes.add(timeTextbox);
        
        Polytempo_Textbox *regionTextbox = new Polytempo_Textbox("Region");
        regionTextbox->setFont(Font (18.0f, Font::plain));
        regionTextbox->addListener(this);
        regionTextbox->setName("Region");
        addChildComponent(regionTextbox);
        regionTextboxes.add(regionTextbox);
        
        TextButton *deleteButton = new TextButton("delete");
        deleteButton->addListener(this);
        addChildComponent(deleteButton);
        deleteButtons.add(deleteButton);
    }
    resized();
}
