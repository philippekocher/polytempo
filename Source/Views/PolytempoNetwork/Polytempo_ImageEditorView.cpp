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

#include "Polytempo_ImageEditorView.h"
#include "../../Application/PolytempoNetwork/Polytempo_NetworkApplication.h"
#include "../../Preferences/Polytempo_StoredPreferences.h"
#include "../../Misc/Polytempo_Alerts.h"

#define TREE_VIEW_WIDTH 200



static ValueTree createTree(const String& image, const int section)
{
    ValueTree t ("Item");
    t.setProperty ("image", image, nullptr);
    t.setProperty ("section", section, nullptr);
    return t;
}

Polytempo_ImageEditorView::Polytempo_ImageEditorView()
{
    addAndMakeVisible(tree = new TreeView());
    addAndMakeVisible(imageEditorViewport = new Polytempo_ImageEditorViewport());
        
    addAndMakeVisible(imageFileTextbox = new Polytempo_Textbox("Image File"));
    imageFileTextbox->setFont(Font (12.0f, Font::plain));
    imageFileTextbox->addListener(this);
    
    addAndMakeVisible(chooseImageFile = new TextButton("Choose.."));
    chooseImageFile->addListener(this);

    addAndMakeVisible(markerTextbox = new Polytempo_Textbox("Marker"));
    markerTextbox->setFont(Font (24.0f, Font::plain));
    markerTextbox->addListener(this);
    
    addAndMakeVisible(timeTextbox = new Polytempo_Textbox("Time"));
    timeTextbox->setFont(Font (24.0f, Font::plain));
    timeTextbox->addListener(this);
    
    addAndMakeVisible(regionTextbox = new Polytempo_Textbox("Region"));
    regionTextbox->setFont(Font (24.0f, Font::plain));
    regionTextbox->addListener(this);
    
    addAndMakeVisible(relativePositionLabel = new Label(String::empty, "Relative Bounds"));

    addAndMakeVisible(xTextbox = new Polytempo_Textbox("Left"));
    xTextbox->setFont(Font (16.0f, Font::plain));
    xTextbox->addListener(this);

    addAndMakeVisible(yTextbox = new Polytempo_Textbox("Top"));
    yTextbox->setFont(Font (16.0f, Font::plain));
    yTextbox->addListener(this);

    addAndMakeVisible(wTextbox = new Polytempo_Textbox("Width"));
    wTextbox->setFont(Font (16.0f, Font::plain));
    wTextbox->addListener(this);

    addAndMakeVisible(hTextbox = new Polytempo_Textbox("Height"));
    hTextbox->setFont(Font (16.0f, Font::plain));
    hTextbox->addListener(this);
}

Polytempo_ImageEditorView::~Polytempo_ImageEditorView()
{
    deleteAllChildren();
}

void Polytempo_ImageEditorView::refresh()
{
    Polytempo_NetworkApplication* const app = dynamic_cast<Polytempo_NetworkApplication*>(JUCEApplication::getInstance());
    score = app->getScore();
    
    chooseImageFile->setEnabled(score != nullptr);
    
    if(score == nullptr) return;
    
    imageEvents = score->getEvents(eventType_Image);
    addRegionEvents = score->getEvents(eventType_AddRegion);
    
    /*
      check size of image regions
      is there a better place to do it?
    */
    for(int i=0;i<imageEvents.size();i++)
    {
        Array < var > r = *imageEvents[i]->getProperty(eventPropertyString_Rect).getArray();
        if(float(r[0])+float(r[2]) > 1.0) // too wide
        {
            r.set(2, 1.0 - float(r[0]));
            imageEvents[i]->setProperty(eventPropertyString_Rect, r);
            score->setDirty();
        }
        if(float(r[1])+float(r[3]) > 1.0) // too high
        {
            r.set(3, 1.0 - float(r[1]));
            imageEvents[i]->setProperty(eventPropertyString_Rect, r);
            score->setDirty();
        }
    }

    ValueTree vt = createTree(String::empty, -1);

    OwnedArray < Image >&  images   = Polytempo_ImageManager::getInstance()->getImages();
    OwnedArray < String >& imageIDs = Polytempo_ImageManager::getInstance()->getImageIDs();
    for(int i=0;i<images.size();i++)
    {
        ValueTree item;
        vt.addChild(item = createTree(*imageIDs.getUnchecked(i), -1), -1, nullptr);

        for(int j=0;j<imageEvents.size();j++)
        {
            if(imageEvents[j]->getProperty("imageID") == *imageIDs.getUnchecked(i))
            {
                item.addChild(createTree(*imageIDs.getUnchecked(i), j), -1, nullptr);
                if(selectedEvent && selectedEvent == imageEvents[j])
                {
                    imageID      = imageIDs.getUnchecked(i);
                    sectionIndex = j;
                    selectedItem = (TreeItem*)tree->getItemOnRow(0);
                    selectedEvent = nullptr;
                }
            }
        }
    }
    
    tree->setDefaultOpenness(true);
    tree->setOpenCloseButtonsVisible(false);
    tree->setMultiSelectEnabled(false);
    tree->setRootItem(rootItem = new TreeItem(vt,this));
    tree->setRootItemVisible(false);
    tree->setColour(TreeView::backgroundColourId, Colour::Colour(245,245,245));
    
    if(selectedItem)
    {
        for(int i=0;i<tree->getNumRowsInTree();i++)
        {
            TreeItem* item = (TreeItem*)tree->getItemOnRow(i);
            if(item->getProperty("image") == imageID &&
               int(item->getProperty("section")) == sectionIndex)
            {
                item->setSelected(true, true);
                selectedItem = item;
                update();
                return;
            }
        }
    }

    if(tree->getNumRowsInTree() > 0)
    {
        TreeItem* item = (TreeItem*)tree->getItemOnRow(0);
        item->setSelected(true, true);
        selectedItem = item;
        update();
    }
}

void Polytempo_ImageEditorView::update()
{
    //DBG("update()");
    imageID = selectedItem->getProperty("image");
    sectionIndex = selectedItem->getProperty("section");

    imageEditorViewport->getComponent()->setImage(Polytempo_ImageManager::getInstance()->getImage(imageID));
    
    imageFileTextbox->setText(Polytempo_ImageManager::getInstance()->getFileName(imageID), dontSendNotification);
    
    if(sectionIndex == -1)
    {
        // hide handles
        imageEditorViewport->getComponent()->setSectionRect(Rectangle<float>(0,0,0,0));
        
        // reset textboxes
        markerTextbox->reset();
        timeTextbox->reset();
        regionTextbox->reset();
        xTextbox->reset();
        yTextbox->reset();
        wTextbox->reset();
        hTextbox->reset();
    }
    else
    {
        imageEditorViewport->getComponent()->setEditedEvent(imageEvents[sectionIndex]);
 
        // handles
        Array<var> r = *imageEvents[sectionIndex]->getProperty(eventPropertyString_Rect).getArray();
        imageEditorViewport->getComponent()->setSectionRect(Rectangle<float>(r[0],r[1],r[2],r[3]));
        
        // textboxes
        String marker;
        if(score->getMarkerForLocator(imageEvents[sectionIndex]->getTime(), &marker))
            markerTextbox->setText(marker, dontSendNotification);
        else
            markerTextbox->reset();
        timeTextbox->setText(Polytempo_Textbox::timeToString(imageEvents[sectionIndex]->getTime()), dontSendNotification);
        regionTextbox->setText(imageEvents[sectionIndex]->getProperty(eventPropertyString_RegionID).toString(),dontSendNotification);
        xTextbox->setText(String((float)r[0],3), dontSendNotification);
        yTextbox->setText(String((float)r[1],3), dontSendNotification);
        wTextbox->setText(String((float)r[2],3), dontSendNotification);
        hTextbox->setText(String((float)r[3],3), dontSendNotification);
        
    }
    repaint();
}

void Polytempo_ImageEditorView::paint(Graphics& g)
{
    g.fillAll(Colour::Colour(245,245,245));
 
    if(tree->getNumSelectedItems() > 0)
    {
        if(tree->getSelectedItem(0) != selectedItem)
        {
            selectedItem = (TreeItem*)tree->getSelectedItem(0);
            update();
        }    
    }
    
    Rectangle<int> r (getLocalBounds());
    
    g.setColour(Colour::Colour(245,245,245));
    g.fillRect(r.removeFromRight(TREE_VIEW_WIDTH));
    g.setColour(Colours::grey);
    g.drawVerticalLine(TREE_VIEW_WIDTH, 0.0f, (float)getHeight());
    g.drawVerticalLine(getWidth() - TREE_VIEW_WIDTH, 0.0f, (float)getHeight());
}

void Polytempo_ImageEditorView::resized()
{
    Rectangle<int> r (getLocalBounds());
    
    tree->setBounds(r.withWidth(TREE_VIEW_WIDTH));
    imageEditorViewport->setBounds(r.withLeft(TREE_VIEW_WIDTH+1).withTrimmedRight(TREE_VIEW_WIDTH));
    
    int yPosition = 25;
    
    imageFileTextbox->setBounds(getWidth() - TREE_VIEW_WIDTH + 10, yPosition, TREE_VIEW_WIDTH - 20, 20);
    yPosition += 25;
 
    chooseImageFile->setBounds(getWidth() - TREE_VIEW_WIDTH + 10, yPosition, 50, 16);
    yPosition += 60;
    
    markerTextbox->setBounds(getWidth() - TREE_VIEW_WIDTH + 10, yPosition, TREE_VIEW_WIDTH - 20, 34);
    yPosition +=55;
    
    timeTextbox->setBounds(getWidth() - TREE_VIEW_WIDTH + 10, yPosition, TREE_VIEW_WIDTH - 20, 34);
    yPosition +=60;
    
    regionTextbox->setBounds(getWidth() - TREE_VIEW_WIDTH + 10, yPosition, 40, 34);
    yPosition +=50;

    relativePositionLabel->setBounds(getWidth() - TREE_VIEW_WIDTH + 5, yPosition, 100, 34);
    yPosition += 45;
    
    xTextbox->setBounds(getWidth() - TREE_VIEW_WIDTH + 10, yPosition, 55, 26);
    yTextbox->setBounds((int)(getWidth() - TREE_VIEW_WIDTH * 0.5 + 10), yPosition, 55, 26);
    yPosition +=45;

    wTextbox->setBounds(getWidth() - TREE_VIEW_WIDTH + 10, yPosition, 55, 26);
    hTextbox->setBounds((int)(getWidth() - TREE_VIEW_WIDTH * 0.5 + 10), yPosition, 55, 26);
}

//------------------------------------------------------------------------------
#pragma mark -
#pragma mark actions

void Polytempo_ImageEditorView::deleteSelected()
{
    imageID = selectedItem->getProperty("image");
    sectionIndex = selectedItem->getProperty("section");
    
    String text("Do you want to delete \"");
    if(sectionIndex == -1) text<<"Image "<<imageID.toString()<<"\"";
    else                   text<<"Section "<<sectionIndex<<"\"";
    
    if(Polytempo_OkCancelAlert::show(text, String::empty))
    {
        if(sectionIndex == -1)
        {
            Polytempo_Alert::show("Sorry...", "\"Delete Image\" is not yet implemented");
        }
        else
        {
            score->removeEvent(imageEvents[sectionIndex]);
            score->setDirty();
        }
        
        selectedItem = nullptr;
        refresh();
    }
}

void Polytempo_ImageEditorView::addImage()
{
    File directory(Polytempo_StoredPreferences::getInstance()->getProps().getValue("scoreFileDirectory"));
    FileChooser fileChooser("Open Score File", directory, "*.jpg;*.png", true);
    
    if(fileChooser.browseForFileToOpen())
    {
        String url = fileChooser.getResult().getRelativePathFrom(directory);
        
        Polytempo_Event *event = Polytempo_Event::makeEvent(eventType_LoadImage);
        event->setProperty(eventPropertyString_URL, url);
        
        int highestImageIndex = 0;
        for(int i=0;i<imageEvents.size();i++)
        {
            int index = imageEvents[i]->getProperty(eventPropertyString_ImageID);
            if(index > highestImageIndex)
                highestImageIndex = index;
        }
        
        event->setProperty(eventPropertyString_ImageID, highestImageIndex + 1);
        
        score->addEvent(event, true); // add to init
        score->setDirty();
        
        refresh();
    }
}

void Polytempo_ImageEditorView::addSection()
{
    Polytempo_Event* event = Polytempo_Event::makeEvent(eventType_Image);
    
    event->setProperty(eventPropertyString_ImageID, imageID);
    
    Array < var > r;
    r.set(0,0); r.set(1,0); r.set(2,1); r.set(3,1);
    event->setProperty(eventPropertyString_Rect, r);
    
    score->addEvent(event);
    score->setDirty();
    
    selectedEvent = event;
    selectedItem = nullptr;
    
    // add a region if there are none
    if(addRegionEvents.size() == 0)
    {
        DBG("add region");
        event->setProperty(eventPropertyString_RegionID, "1");
        
        event = Polytempo_Event::makeEvent(eventType_AddRegion);
        event->setProperty(eventPropertyString_RegionID, "1");
        event->setProperty(eventPropertyString_Rect, r);

        score->addEvent(event, true); // add to init
    }
    
    refresh();
}

//------------------------------------------------------------------------------
#pragma mark -
#pragma mark label listener

void Polytempo_ImageEditorView::editorShown(Label* label, TextEditor&)
{}

void Polytempo_ImageEditorView::labelTextChanged(Label* label)
{
    if(label == imageFileTextbox)
    {
        Polytempo_ImageManager::getInstance()->replaceImage(imageID, label->getTextValue());
        score->setDirty();
        refresh();
    }
    else if(label == markerTextbox)
    {
        String marker = label->getTextValue().toString();
        float locator;
        
        if(score->getLocatorForMarker(marker, &locator))
        {
            imageEvents[sectionIndex]->setTime(locator);
            score->sortSection();
            score->setDirty();
        }

        update();
    }
    else if(label == timeTextbox)
    {
        float num = Polytempo_Textbox::stringToTime(label->getText());
        
        imageEvents[sectionIndex]->setTime(num);
        score->sortSection();
        score->setDirty();
        
        update();
    }
    else if(label == regionTextbox)
    {
        int num = (label->getText()).getIntValue();
        // TODO: check if region exists
        
        imageEvents[sectionIndex]->setProperty(eventPropertyString_RegionID, var(num));
        score->setDirty();
        
        update();
    }
    else if(label == xTextbox || label == yTextbox || label == wTextbox || label == hTextbox)
    {
        Array < var > r = *imageEvents[sectionIndex]->getProperty(eventPropertyString_Rect).getArray();
        imageEditorViewport->getComponent()->setSectionRect(Rectangle<float>(r[0],r[1],r[2],r[3]));
        float num = label->getText().getFloatValue();
        num = num < 0.0f ? 0.0f : num > 1.0f ? 1.0f : num;
        
        if     (label == xTextbox) r.set(0, num);
        else if(label == yTextbox) r.set(1, num);
        else if(label == wTextbox) r.set(2, num);
        else if(label == hTextbox) r.set(3, num);
        
        imageEvents[sectionIndex]->setProperty(eventPropertyString_Rect, r);
        score->setDirty();
        
        update();
    }
}

//------------------------------------------------------------------------------
#pragma mark -
#pragma mark button listener

void Polytempo_ImageEditorView::buttonClicked(Button* button)
{
    if(button == chooseImageFile)
    {
        File directory (Polytempo_StoredPreferences::getInstance()->getProps().getValue("scoreFileDirectory"));
        FileChooser fileChooser ("Open Score File", directory, "*.png;*.jpg");
        
        if(fileChooser.browseForFileToOpen())
        {
            Polytempo_ImageManager::getInstance()->replaceImage(imageID, fileChooser.getResult().getRelativePathFrom(directory));
            score->setDirty();
            refresh();
        }
    }
}

//------------------------------------------------------------------------------
#pragma mark -
#pragma mark event observer

void Polytempo_ImageEditorView::eventNotification(Polytempo_Event *event)
{
    if(event->getType() == eventType_DeleteAll)
    {
    }
    if(event->getType() == eventType_Ready && isVisible())
    {
        selectedItem = nullptr;
        refresh();
    }
}



