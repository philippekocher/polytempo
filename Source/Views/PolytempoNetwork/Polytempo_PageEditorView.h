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

#pragma once

#include "../../Scheduler/Polytempo_EventObserver.h"
#include "../../Misc/Polytempo_Textbox.h"
#include "../../Data/Polytempo_Score.h"
#include "Polytempo_ImageManager.h"
#include "Polytempo_PageEditorComponent.h"
#include "Polytempo_SectionInstancesComponent.h"


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

class Polytempo_PageEditorViewport : public Viewport
{
public:
    Polytempo_PageEditorViewport()
    {
        setViewedComponent(component = new Polytempo_PageEditorComponent());
    }
    ~Polytempo_PageEditorViewport() {}
    Polytempo_PageEditorComponent* getComponent() { return component; }

private:
    Polytempo_PageEditorComponent *component;
};

class Polytempo_SectionInstancesViewport : public Viewport
{
public:
    Polytempo_SectionInstancesViewport()
    {
        setViewedComponent(component = new Polytempo_SectionInstancesComponent());
    }
    ~Polytempo_SectionInstancesViewport() {}
    Polytempo_SectionInstancesComponent* getComponent() { return component; }
    void resized() { component->setBounds(getBounds()); }
    
private:
    Polytempo_SectionInstancesComponent *component;
};

class Polytempo_PageEditorView : public Component,
                                 public Label::Listener,
                                 public Button::Listener,
                                 public Polytempo_EventObserver
{
public:
    Polytempo_PageEditorView();
    ~Polytempo_PageEditorView();
    
    void refresh();
    void update();
    
    void paint(Graphics&) override;
    void resized() override;
    
    // actions
#ifdef JUCE_ANDROID
	void mouseDoubleClick(const MouseEvent &) override;
#else
	void mouseDown(const MouseEvent &) override; 
#endif

    void deleteSelected();
	void performDeleteSelected();
    void loadImage();
    void addSection();
    void addInstance();
    
    // retrieve state
    bool hasSelectedImage();
    bool hasSelectedSection();

    // Label Listener
    void editorShown(Label* label, TextEditor&) override;
    void labelTextChanged(Label* labelThatHasChanged) override;
    
    // Change Listener
    void changeListenerCallback(ChangeBroadcaster*);
    
    // Button Listener
    void buttonClicked(Button*) override;
    
    // Event Observer
    void eventNotification(Polytempo_Event *event) override;
    
private:
    int findNewID(String eventPropertyString, Array < Polytempo_Event* > events);
    void loadImage(File file);
    void showMenu();

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
    
    Polytempo_PageEditorViewport* pageEditorViewport;
    
    Label *imageFileLabel;
    Polytempo_Textbox *imageFileTextbox;
    TextButton *chooseImageFile;
    
    Label *relativePositionLabel;
    Polytempo_Textbox *xTextbox;
    Polytempo_Textbox *yTextbox;
    Polytempo_Textbox *wTextbox;
    Polytempo_Textbox *hTextbox;
    
    Label *sectionInstancesLabel;
    Polytempo_SectionInstancesViewport* sectionInstancesViewport;
    
    ScopedPointer<FileChooser> fc;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_PageEditorView)
};
