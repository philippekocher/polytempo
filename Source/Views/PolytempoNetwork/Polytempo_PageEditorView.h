#pragma once

#include "../../Scheduler/Polytempo_EventObserver.h"
#include "../../Misc/Polytempo_Textbox.h"
#include "../../Data/Polytempo_Score.h"
#include "Polytempo_NetworkView.h"
#include "Polytempo_ImageManager.h"
#include "Polytempo_PageEditorComponent.h"
#include "Polytempo_SectionInstancesComponent.h"


class TreeItem : public TreeViewItem, private ValueTree::Listener
{
public:
    TreeItem(const ValueTree& theTree, Component* theOwner)
    : tree (theTree), owner (theOwner)
    {
    }
    
    void setTree(const ValueTree& theTree)
    {
        tree = theTree;
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
        
        if(*image == Image())
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
        if(tree["sectionID"] == var()) text << "Image " << tree["imageID"].toString();
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
                                 public Polytempo_NetworkView,
                                 public Label::Listener,
                                 public Button::Listener,
                                 public Polytempo_EventObserver
{
public:
    Polytempo_PageEditorView();
    ~Polytempo_PageEditorView() override;
    
    void refresh();
    void update();
    
    void paint(Graphics&) override;
    void resized() override;
    
    // actions
#if defined JUCE_ANDROID || JUCE_IOS
	void mouseDoubleClick(const MouseEvent &) override;
#else
	void mouseDown(const MouseEvent &) override; 
#endif

    void deleteSelected() override;
	void performDeleteSelected();
    void loadImage();
    void addSection(int imgID = 0);
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
	std::unique_ptr < TreeItem > rootItem;
    TreeItem *selectedItem;
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
    TextButton * addSectionButton;
    Polytempo_SectionInstancesViewport* sectionInstancesViewport;
    
	std::unique_ptr<FileChooser> fc;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_PageEditorView)
};
