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


#include "Polytempo_ImageEditorComponent.h"
#include "../../Preferences/Polytempo_StoredPreferences.h"
#include "../../Application/PolytempoNetwork/Polytempo_NetworkApplication.h"
#include "Polytempo_ImageEditorView.h"


Polytempo_ImageEditorComponent::Polytempo_ImageEditorComponent()
{
	image = nullptr;

    for (int i=0;i<4;i++)
    {
        DragHandle* dh = new DragHandle(this);
        dh->setConstraint(i%2==0 ? 2 : 1);
        dragHandles.add(dh);
        addChildComponent(dh);
    }
    
    zoomFactor = (float)Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("zoom");
    Polytempo_StoredPreferences::getInstance()->getProps().addChangeListener(this);
}

Polytempo_ImageEditorComponent::~Polytempo_ImageEditorComponent()
{
    deleteAllChildren();
}

void Polytempo_ImageEditorComponent::update()
{}

void Polytempo_ImageEditorComponent::paint(Graphics& g)
{
    g.fillAll (Colours::red);

    if(image != nullptr)
    {
        g.drawImage(*image,
                    0, 0, (int)(image->getWidth() * zoomFactor), (int)(image->getHeight() * zoomFactor),
                    0, 0, image->getWidth(), image->getHeight());
        
        if(!sectionRect.isEmpty())
        {
            g.setColour (Colours::orangered);
            
            g.drawRect(dragHandles.getUnchecked(3)->getCentreX(),
                       dragHandles.getUnchecked(0)->getCentreY(),
                       dragHandles.getUnchecked(1)->getCentreX() - dragHandles.getUnchecked(3)->getCentreX(),
                       dragHandles.getUnchecked(2)->getCentreY() - dragHandles.getUnchecked(0)->getCentreY(),
                       1.0);
        }
    }
}

void Polytempo_ImageEditorComponent::resized()
{
    if(image && image->isValid())
    {
        setBounds(0, 0, (int)(image->getWidth() * zoomFactor), (int)(image->getHeight() * zoomFactor));
    
        if(dragHandles.getUnchecked(0)->isVisible())
        {
            dragHandles.getUnchecked(0)->setCentrePosition((int)((sectionRect.getX() + sectionRect.getWidth() * 0.5) * image->getWidth() * zoomFactor), (int)(sectionRect.getY() * image->getHeight() * zoomFactor));
            
            dragHandles.getUnchecked(1)->setCentrePosition((int)((sectionRect.getX()+sectionRect.getWidth()) * image->getWidth() * zoomFactor), (int)((sectionRect.getY() + sectionRect.getHeight() * 0.5) * image->getHeight() * zoomFactor));
            
            dragHandles.getUnchecked(2)->setCentrePosition((int)((sectionRect.getX() + sectionRect.getWidth() * 0.5) * image->getWidth() * zoomFactor), (int)((sectionRect.getY()+sectionRect.getHeight()) * image->getHeight() * zoomFactor));
            
            dragHandles.getUnchecked(3)->setCentrePosition((int)(sectionRect.getX() * image->getWidth() * zoomFactor), (int)((sectionRect.getY() + sectionRect.getHeight() * 0.5) * image->getHeight() * zoomFactor));
        }
    }
}

void Polytempo_ImageEditorComponent::setImage(Image* img)
{
    image = img;
    resized();
}

void Polytempo_ImageEditorComponent::setSectionRect(Rectangle < float > r)
{
    sectionRect = r;
    
    if(sectionRect.getWidth() == 0 || sectionRect.getHeight() == 0)
    {
        for (int i=0;i<4;i++) { dragHandles.getUnchecked(i)->setVisible(false); }
    }
    else
    {
        for (int i=0;i<4;i++)
        {
            dragHandles.getUnchecked(i)->setVisible(true);
            dragHandles.getUnchecked(i)->setBoundsConstraint(getBounds());
        }
        resized();
    }
}

void Polytempo_ImageEditorComponent::setEditedEvent(Polytempo_Event *event)
{
    if(event->getType() == eventType_Image) editedEvent = event;
    
    Polytempo_NetworkApplication* const app = dynamic_cast<Polytempo_NetworkApplication*>(JUCEApplication::getInstance());
    score = app->getScore();
    
}

//------------------------------------------------------------------------------
#pragma mark -
#pragma mark drag handle listener

void Polytempo_ImageEditorComponent::draggingSessionEnded()
{}

void Polytempo_ImageEditorComponent::positionChanged(DragHandle* handle)
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
    r.add((dragHandles.getUnchecked(3)->getCentreX()) / image->getWidth() / zoomFactor);
    r.add(dragHandles.getUnchecked(0)->getCentreY() / image->getHeight() / zoomFactor);
    r.add((dragHandles.getUnchecked(1)->getCentreX() - dragHandles.getUnchecked(3)->getCentreX()) / image->getWidth() / zoomFactor);
    r.add((dragHandles.getUnchecked(2)->getCentreY() - dragHandles.getUnchecked(0)->getCentreY()) / image->getHeight() / zoomFactor);

    if(editedEvent)
    {
        editedEvent->setProperty(eventPropertyString_Rect, r);
        score->setDirty();
    }
    
    // update everything
    Component *parent = getParentComponent()->getParentComponent()->getParentComponent();
    ((Polytempo_ImageEditorView*)parent)->update();
}


//------------------------------------------------------------------------------
#pragma mark -
#pragma mark change listener

void Polytempo_ImageEditorComponent::changeListenerCallback(ChangeBroadcaster*)
{
    if(!isVisible()) return;
    
    if(zoomFactor != Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("zoom"))
    {
        zoomFactor = (float)Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("zoom");
        resized();
    }
}


