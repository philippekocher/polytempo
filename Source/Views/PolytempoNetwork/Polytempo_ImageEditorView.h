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

#ifndef __Polytempo_ImageEditorView__
#define __Polytempo_ImageEditorView__

#include "../../Scheduler/Polytempo_EventObserver.h"
#include "../../Misc/Polytempo_Textbox.h"
#include "../../Data/Polytempo_Score.h"
#include "Polytempo_ImageManager.h"
#include "Polytempo_ImageEditorComponent.h"


class TreeItem : public TreeViewItem, private ValueTree::Listener
{
public:
    TreeItem(const ValueTree& theTree, Component* theOwner)
    : tree (theTree), owner (theOwner)
    {
        //tree.addListener (this);
    }
    
    bool mightContainSubItems() override
    {
        return tree.getNumChildren() > 0;
    }
    
    void itemOpennessChanged(bool) override
    {
        clearSubItems();
        
        for (int i = 0; i < tree.getNumChildren(); ++i)
            addSubItem (new TreeItem(tree.getChild (i), owner));
        
        owner->repaint();
    }
    
    void itemSelectionChanged(bool) override
    {
        owner->repaint();
    }
    
    var getProperty(const String& key)
    {
        return tree.getProperty(key);
    }
    
    void paintItem(Graphics& g, int width, int height) override
    {
        Image *image = Polytempo_ImageManager::getInstance()->getImage(tree["imageID"].toString());
        
        if(image == nullptr) return;
        
        if(*image == Image::null)
        {
            g.setColour(Colours::grey);
            g.setFont(Font (15.0000f, Font::italic));
        }
        else
        {
            g.setColour(Colours::black);
            g.setFont(15.0f);
        }
        
        String text = "";
        if(tree["sectionID"] == var::null) text << "Image " << tree["imageID"].toString();
        else                               text << "Section " << tree["sectionID"].toString();
        
        g.drawText(text,
                   10, 0, width - 4, height,
                   Justification::centredLeft, true);
    }
private:
    ValueTree tree;
    Component *owner;
    
    void valueTreePropertyChanged (ValueTree&, const Identifier&) override
    {
        repaintItem();
    }
    
    void valueTreeChildAdded (ValueTree&, ValueTree&) override {}
    void valueTreeChildRemoved (ValueTree&, ValueTree&, int) override {}
    void valueTreeChildOrderChanged (ValueTree&, int, int) override {}
    void valueTreeParentChanged (ValueTree&) override {}
    
};

class Polytempo_ImageEditorViewport : public Viewport
{
public:
    Polytempo_ImageEditorViewport()
    {
        setViewedComponent(component = new Polytempo_ImageEditorComponent());
    }
    ~Polytempo_ImageEditorViewport() {}
    Polytempo_ImageEditorComponent* getComponent() { return component; }

private:
    Polytempo_ImageEditorComponent *component;
};


class Polytempo_ImageEditorView : public Component,
                                  public Label::Listener,
                                  public ButtonListener,
                                  public Polytempo_EventObserver
{
public:
    Polytempo_ImageEditorView();
    ~Polytempo_ImageEditorView();
    
    void refresh();
    void update();
    
    void paint(Graphics&);
    void resized();
    
    // actions
    void deleteSelected();
    void loadImage();
    void addSection();
    void addInstance();
    
    // retrieve state
    bool hasSelectedSection();

    // Label Listener
    void editorShown(Label* label, TextEditor&);
    void labelTextChanged(Label* labelThatHasChanged);
    
    // Change Listener
    void changeListenerCallback(ChangeBroadcaster*);
    
    // Button Listener
    void buttonClicked(Button*);
    
    // Event Observer
    void eventNotification(Polytempo_Event *event);
    
private:
    int findNewID(String eventPropertyString, Array < Polytempo_Event* > events);
    
    
    TreeView *tree;
    ScopedPointer < TreeItem > rootItem;
    TreeItem *selectedItem;
    Polytempo_Event *selectedEvent;
    var imageID;
    var sectionID;
    
    Polytempo_Score *score;
    
    Array < Polytempo_Event* > addRegionEvents;
    Array < Polytempo_Event* > addSectionEvents;
    Array < Polytempo_Event* > loadImageEvents;
    Array < Polytempo_Event* > imageEvents;
    
    Polytempo_Event *selectedAddSectionEvent;
    Polytempo_Event *selectedImageEvent;
    
    Polytempo_ImageEditorViewport* imageEditorViewport;
    
    Label *imageFileLabel;
    Polytempo_Textbox *imageFileTextbox;
    TextButton *chooseImageFile;
    
    Polytempo_Textbox *markerTextbox;
    Polytempo_Textbox *timeTextbox;
    Polytempo_Textbox *regionTextbox;
    
    Label *relativePositionLabel;
    Polytempo_Textbox *xTextbox;
    Polytempo_Textbox *yTextbox;
    Polytempo_Textbox *wTextbox;
    Polytempo_Textbox *hTextbox;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_ImageEditorView)
};

#endif  // __Polytempo_ImageEditorView__
