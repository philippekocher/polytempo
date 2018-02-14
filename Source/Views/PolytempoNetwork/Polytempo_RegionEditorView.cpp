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

#include "Polytempo_RegionEditorView.h"
#include "../../Application/PolytempoNetwork/Polytempo_NetworkApplication.h"

Polytempo_RegionEditorView::Polytempo_RegionEditorView()
{
    for (int i=0;i<4;i++)
    {
        DragHandle* dh = new DragHandle(this);
        dh->setConstraint(i%2==0 ? 2 : 1);
        dragHandles.add(dh);
        addChildComponent(dh);
    }
}

Polytempo_RegionEditorView::~Polytempo_RegionEditorView()
{
    deleteAllChildren();
}

void Polytempo_RegionEditorView::paint(Graphics& g)
{
    g.fillAll(Colours::white);
    
    int width = getWidth();
    int height = getHeight();
    int i = 0;
    
    if(selectedRegionID < addRegionEvents.size())
        for(int i=0;i<4;i++) dragHandles.getUnchecked(i)->setVisible(true);
    else
        for(int i=0;i<4;i++) dragHandles.getUnchecked(i)->setVisible(false);

    for(Polytempo_Event *addRegionEvent : addRegionEvents)
    {
        Array<var> r = *addRegionEvent->getProperty(eventPropertyString_Rect).getArray();
        
        Rectangle<int> bounds = Rectangle<int>(width * float(r[0]), height * float(r[1]), width * float(r[2]), height * float(r[3]));
        
        if(i++ == selectedRegionID)
        {
            g.setColour(Colours::blue.withAlpha(0.05f));
            g.fillRect(bounds);

            dragHandles.getUnchecked(0)->setCentrePosition(int((bounds.getX() + bounds.getWidth() * 0.5f)), int(bounds.getY()));
            
            dragHandles.getUnchecked(1)->setCentrePosition(int((bounds.getX()+bounds.getWidth())), int((bounds.getY() + bounds.getHeight() * 0.5f)));
            
            dragHandles.getUnchecked(2)->setCentrePosition(int((bounds.getX() + bounds.getWidth() * 0.5f)), int((bounds.getY()+bounds.getHeight())));
            
            dragHandles.getUnchecked(3)->setCentrePosition(int(bounds.getX()), int((bounds.getY() + bounds.getHeight() * 0.5f)));

            g.setColour(Colours::blue);
            g.drawRect(bounds, 2.0f);
            
            if(bounds.getHeight() < bounds.getWidth() * 2)
                g.setFont(bounds.getHeight());
            else
                g.setFont(bounds.getWidth() * 2);
            g.setColour(Colours::blue.withAlpha(0.2f));
            g.drawFittedText(addRegionEvent->getProperty(eventPropertyString_RegionID).toString(), bounds, Justification::centred, 0.1f);
        }
        else
        {
            g.setColour(Colours::blue.withAlpha(0.7f));
            g.drawRect(width * float(r[0]) + 1, height * float(r[1]) + 1, width * float(r[2]) - 2, height * float(r[3]) - 2, 1.0f);

            if(bounds.getHeight() < bounds.getWidth() * 2)
                g.setFont(bounds.getHeight());
            else
                g.setFont(bounds.getWidth() * 2);
            g.setColour(Colours::blue.withAlpha(0.05f));
            g.drawFittedText(addRegionEvent->getProperty(eventPropertyString_RegionID).toString(), bounds, Justification::centred, 0.1f);
}
    }
}

void Polytempo_RegionEditorView::refresh()
{
    Polytempo_NetworkApplication* const app = dynamic_cast<Polytempo_NetworkApplication*>(JUCEApplication::getInstance());
    score = app->getScore();
        
    if(score == nullptr) return;
    
    addRegionEvents = score->getEvents(eventType_AddRegion);
    selectedRegionID = 0;
    repaint();
}

bool Polytempo_RegionEditorView::keyPressed(const KeyPress& key)
{
    return false;
}

void Polytempo_RegionEditorView::mouseDown(const MouseEvent& event)
{
    int i = 0;
    int width = getWidth();
    int height = getHeight();

    for(Polytempo_Event *addRegionEvent : addRegionEvents)
    {
        Array<var> r = *addRegionEvent->getProperty(eventPropertyString_Rect).getArray();
        Rectangle<int> bounds = Rectangle<int>(width * float(r[0]), height * float(r[1]), width * float(r[2]), height * float(r[3]));
        
        if(bounds.contains(event.getPosition()))
            break;
        
        i++;
    }
    if(selectedRegionID != i)
    {
        selectedRegionID = i;
        repaint();
    }
}

void Polytempo_RegionEditorView::addRegion()
{
    // find new id
    int newID = 0;
    bool success = false;
    
    while(!success)
    {
        newID++;
        success = true;
        for(Polytempo_Event *addRegionEvent : addRegionEvents)
        {
            if(addRegionEvent->getProperty(eventPropertyString_RegionID).equals(var(newID)))
            {
                success = false;
                break;
            }
        }
    }

    Polytempo_Event* event = Polytempo_Event::makeEvent(eventType_AddRegion);
    Array < var > r;
    r.set(0,0); r.set(1,0); r.set(2,1); r.set(3,1);
    event->setProperty(eventPropertyString_Rect, r);
    event->setProperty(eventPropertyString_RegionID, var(newID));

    score->addEvent(event, true);
    score->setDirty();
    
    addRegionEvents.add(event);
    selectedRegionID = addRegionEvents.size() - 1;
    
    repaint();
}

//------------------------------------------------------------------------------
#pragma mark -
#pragma mark drag handle listener

void Polytempo_RegionEditorView::draggingSessionEnded()
{}

void Polytempo_RegionEditorView::positionChanged(DragHandle* handle)
{
    if(handle != dragHandles.getUnchecked(0))
        dragHandles.getUnchecked(0)->setCentrePosition((int)(dragHandles.getUnchecked(3)->getCentreX() + (dragHandles.getUnchecked(1)->getCentreX() - dragHandles.getUnchecked(3)->getCentreX()) * 0.5), (int)(dragHandles.getUnchecked(0)->getCentreY()));
    
    if(handle != dragHandles.getUnchecked(1))
        dragHandles.getUnchecked(1)->setCentrePosition((int)(dragHandles.getUnchecked(1)->getCentreX()), (int)(dragHandles.getUnchecked(2)->getCentreY() + (dragHandles.getUnchecked(0)->getCentreY() - dragHandles.getUnchecked(2)->getCentreY()) * 0.5));
    
    if(handle != dragHandles.getUnchecked(2))
        dragHandles.getUnchecked(2)->setCentrePosition((int)(dragHandles.getUnchecked(3)->getCentreX() + (dragHandles.getUnchecked(1)->getCentreX() - dragHandles.getUnchecked(3)->getCentreX()) * 0.5), (int)(dragHandles.getUnchecked(2)->getCentreY()));
    
    if(handle != dragHandles.getUnchecked(3))
        dragHandles.getUnchecked(3)->setCentrePosition((int)(dragHandles.getUnchecked(3)->getCentreX()), (int)(dragHandles.getUnchecked(2)->getCentreY() + (dragHandles.getUnchecked(0)->getCentreY() - dragHandles.getUnchecked(2)->getCentreY()) * 0.5));
    
    Array<var> r;
    r.add((dragHandles.getUnchecked(3)->getCentreX()) / getWidth());
    r.add(dragHandles.getUnchecked(0)->getCentreY() / getHeight());
    r.add((dragHandles.getUnchecked(1)->getCentreX() - dragHandles.getUnchecked(3)->getCentreX()) / getWidth());
    r.add((dragHandles.getUnchecked(2)->getCentreY() - dragHandles.getUnchecked(0)->getCentreY()) / getHeight());
    
    addRegionEvents.getUnchecked(selectedRegionID)->setProperty(eventPropertyString_Rect, r);
    score->setDirty();
    
    repaint();
}

//------------------------------------------------------------------------------
#pragma mark -
#pragma mark event observer

void Polytempo_RegionEditorView::eventNotification(Polytempo_Event *event)
{
    if(event->getType() == eventType_Ready && isVisible())
    {
        refresh();
    }
}


