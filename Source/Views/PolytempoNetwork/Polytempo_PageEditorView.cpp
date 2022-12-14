#include "Polytempo_PageEditorView.h"
#include "../../Application/PolytempoNetwork/Polytempo_NetworkApplication.h"
#include "../../Application/Polytempo_CommandIDs.h"
#include "../../Preferences/Polytempo_StoredPreferences.h"
#include "../../Scheduler/Polytempo_EventScheduler.h"
#include "../../Misc/Polytempo_Alerts.h"

#define TREE_VIEW_WIDTH 200



static ValueTree createTree(const String& imageID, const String& sectionID)
{
    ValueTree t ("Item");
    t.setProperty ("imageID", imageID, nullptr);
    t.setProperty ("sectionID", sectionID, nullptr);
    return t;
}

Polytempo_PageEditorView::Polytempo_PageEditorView()
{
    addAndMakeVisible(tree = new TreeView());
    tree->addMouseListener(this, false);
    
    addAndMakeVisible(pageEditorViewport = new Polytempo_PageEditorViewport());
    pageEditorViewport->addMouseListener(this, true);
    
    addChildComponent(imageFileLabel = new Label(String(), "Image File"));
    imageFileLabel->setFont(Font (15.0f, Font::plain));
    
    addChildComponent(imageFileTextbox = new Polytempo_Textbox(String()));
    imageFileTextbox->setFont(Font (14.0f, Font::plain));
    imageFileTextbox->addListener(this);
    
    addChildComponent(chooseImageFile = new TextButton("Choose.."));
    chooseImageFile->addListener(this);

    addChildComponent(relativePositionLabel = new Label(String(), "Relative Bounds"));
    relativePositionLabel->setFont(Font (15.0f, Font::plain));

    addChildComponent(xTextbox = new Polytempo_Textbox("Left"));
    xTextbox->setFont(Font (16.0f, Font::plain));
    xTextbox->addListener(this);

    addChildComponent(yTextbox = new Polytempo_Textbox("Top"));
    yTextbox->setFont(Font (16.0f, Font::plain));
    yTextbox->addListener(this);

    addChildComponent(wTextbox = new Polytempo_Textbox("Width"));
    wTextbox->setFont(Font (16.0f, Font::plain));
    wTextbox->addListener(this);

    addChildComponent(hTextbox = new Polytempo_Textbox("Height"));
    hTextbox->setFont(Font (16.0f, Font::plain));
    hTextbox->addListener(this);
    
    addChildComponent(sectionInstancesLabel = new Label(String(), "Instances"));
    sectionInstancesLabel->setFont(Font (15.0f, Font::plain));
    
    addChildComponent(sectionInstancesViewport = new Polytempo_SectionInstancesViewport());
    sectionInstancesViewport->setScrollBarsShown(true, false);
    sectionInstancesViewport->getComponent()->setModel(this);
    sectionInstancesViewport->addMouseListener(this, true);

    selectedItem = nullptr;
}

Polytempo_PageEditorView::~Polytempo_PageEditorView()
{
    deleteAllChildren();
}

void Polytempo_PageEditorView::refresh()
{
    const MessageManagerLock mml(Thread::getCurrentThread());

    Polytempo_NetworkApplication* const app = dynamic_cast<Polytempo_NetworkApplication*>(JUCEApplication::getInstance());
    score = app->getScore();
    
    chooseImageFile->setEnabled(score != nullptr);
    
    if(score == nullptr) return;
    
    addRegionEvents = score->getEvents(eventType_AddRegion);
    addSectionEvents = score->getEvents(eventType_AddSection);
    loadImageEvents = score->getEvents(eventType_LoadImage);
    imageEvents = score->getEvents(eventType_Image);
    
    /*
      check size of image regions
      is there a better place to do it?
    */
    for(int i=0;i<addSectionEvents.size();i++)
    {
        Array < var > r;
        
        if(addSectionEvents[i]->hasProperty(eventPropertyString_Rect))
        {
           r = *addSectionEvents[i]->getProperty(eventPropertyString_Rect).getArray();
            if(float(r[0])+float(r[2]) > 1.0) // too wide
            {
                r.set(2, 1.0f - float(r[0]));
                addSectionEvents[i]->setProperty(eventPropertyString_Rect, r);
                score->setDirty();
            }
            if(float(r[1])+float(r[3]) > 1.0) // too high
            {
                r.set(3, 1.0f - float(r[1]));
                addSectionEvents[i]->setProperty(eventPropertyString_Rect, r);
                score->setDirty();
            }
        }
    }

    // create and populate tree
    ValueTree vt = createTree(String(), String());

    for(int i=0;i<loadImageEvents.size();i++)
    {
        ValueTree item;
        vt.addChild(item = createTree(loadImageEvents[i]->getProperty(eventPropertyString_ImageID), String()), -1, nullptr);
        
        if(selectedEvent && selectedEvent == loadImageEvents[i])
        {
            imageID       = loadImageEvents[i]->getProperty(eventPropertyString_ImageID);
            sectionID     = var();
            selectedItem  = (TreeItem*)tree->getItemOnRow(0);
            selectedEvent = nullptr;
        }

        for(int j=0;j<addSectionEvents.size();j++)
        {
            if(addSectionEvents[j]->getProperty(eventPropertyString_ImageID) == loadImageEvents[i]->getProperty(eventPropertyString_ImageID))
            {
                item.addChild(createTree(loadImageEvents[i]->getProperty(eventPropertyString_ImageID), addSectionEvents[j]->getProperty(eventPropertyString_SectionID)), -1, nullptr);
                if(selectedEvent && selectedEvent == addSectionEvents[j])
                {
                    imageID       = loadImageEvents[i]->getProperty(eventPropertyString_ImageID);
                    sectionID     = selectedEvent->getProperty(eventPropertyString_SectionID);
                    selectedItem  = (TreeItem*)tree->getItemOnRow(0);
                    selectedEvent = nullptr;
                }
            }
        }
    }
 
    if(rootItem == nullptr)  rootItem.reset(new TreeItem(vt, this));
    else                     rootItem->setTree(vt);

    tree->setDefaultOpenness(true);
    tree->setOpenCloseButtonsVisible(false);
    tree->setMultiSelectEnabled(false);
    tree->setRootItem(rootItem.get());
    tree->setRootItemVisible(false);
    tree->setColour(TreeView::backgroundColourId, Colour(245,245,245));
    
    if(selectedItem)
    {
        for(int i=0;i<tree->getNumRowsInTree();i++)
        {
            TreeItem* item = (TreeItem*)tree->getItemOnRow(i);
            if(item->getProperty(eventPropertyString_ImageID) == imageID)  // imageID matches
            {
                item->setSelected(true, true);
                selectedItem = item;

                if(item->getProperty(eventPropertyString_SectionID) == sectionID)
                {
                    // both the imageID and the sectionID match
                    // it can't get any better
                    break;
                }
            }
        }
    }
    else if(tree->getNumRowsInTree() > 0)
    {
        TreeItem* item = (TreeItem*)tree->getItemOnRow(0);
        item->setSelected(true, true);
        selectedItem = item;
    }
    
    update();
}

void Polytempo_PageEditorView::update()
{
//    DBG("update()");
    if(selectedItem)
    {
        imageID = selectedItem->getProperty("imageID");
        sectionID = selectedItem->getProperty("sectionID");
    }
    else
    {
        imageID = var();
        sectionID = var();
    }

    pageEditorViewport->getComponent()->setImage(Polytempo_ImageManager::getInstance()->getImage(imageID));

    selectedAddSectionEvent = nullptr;
    selectedImageEvent = nullptr;

    // hide handles
    pageEditorViewport->getComponent()->setSectionRect(Rectangle<float>(0,0,0,0));
    
    // hide image texboxes
    imageFileLabel->setVisible(false);
    imageFileTextbox->setVisible(false);
    chooseImageFile->setVisible(false);

    // hide section textboxes
    relativePositionLabel->setVisible(false);
    xTextbox->setVisible(false);
    yTextbox->setVisible(false);
    wTextbox->setVisible(false);
    hTextbox->setVisible(false);

    // hide section instances
    sectionInstancesLabel->setVisible(false);
    sectionInstancesViewport->setVisible(false);
        
    if(imageID != var() && sectionID == var())
    {
        // show textboxes
        imageFileLabel->setVisible(true);
        imageFileTextbox->setVisible(true);
        imageFileTextbox->setText(Polytempo_ImageManager::getInstance()->getFileName(imageID), dontSendNotification);
        chooseImageFile->setVisible(true);
    }
    else if(sectionID != var())
    {
        for(int i=0;i<addSectionEvents.size();i++)
        {
            if(addSectionEvents[i]->getProperty(eventPropertyString_SectionID) == sectionID)
            {
                selectedAddSectionEvent = addSectionEvents[i];
                break;
            }
        }
        
        pageEditorViewport->getComponent()->setEditedEvent(selectedAddSectionEvent);
 
        // show handles
        Array<var> r;
        if(selectedAddSectionEvent->hasProperty(eventPropertyString_Rect))
        {
            r = *selectedAddSectionEvent->getProperty(eventPropertyString_Rect).getArray();
        }
        else
        {
            r = Polytempo_Event::defaultRectangle();
        }

        pageEditorViewport->getComponent()->setSectionRect(Rectangle<float>(r[0],r[1],r[2],r[3]));

        // show textboxes
        relativePositionLabel->setVisible(true);
        xTextbox->setVisible(true);
        yTextbox->setVisible(true);
        wTextbox->setVisible(true);
        hTextbox->setVisible(true);
        xTextbox->setText(String((float)r[0],3), dontSendNotification);
        yTextbox->setText(String((float)r[1],3), dontSendNotification);
        wTextbox->setText(String((float)r[2],3), dontSendNotification);
        hTextbox->setText(String((float)r[3],3), dontSendNotification);
        
        sectionInstancesLabel->setVisible(true);
        sectionInstancesViewport->setVisible(true);
        sectionInstancesViewport->getComponent()->setImageEvents(imageEvents, sectionID);
    }
    
    repaint();
    
    // enable / disable menu items
    Polytempo_NetworkApplication* const app = dynamic_cast<Polytempo_NetworkApplication*>(JUCEApplication::getInstance());
    app->commandStatusChanged();
}

void Polytempo_PageEditorView::paint(Graphics& g)
{
    g.fillAll(Colour(245,245,245));
 
    if(tree->getNumSelectedItems() > 0)
    {
        if(tree->getSelectedItem(0) != selectedItem)
        {
            selectedItem = (TreeItem*)tree->getSelectedItem(0);
            update();
        }    
    }
    
    Rectangle<int> r (getLocalBounds());
    
    g.setColour(Colour(245,245,245));
    g.fillRect(r.removeFromRight(TREE_VIEW_WIDTH));
    g.setColour(Colours::grey);
    if(sectionID != var())
        g.drawHorizontalLine(168, float(getWidth() - TREE_VIEW_WIDTH), float(getWidth()));
    g.drawVerticalLine(TREE_VIEW_WIDTH, 0.0f, float(getHeight()));
    g.drawVerticalLine(getWidth() - TREE_VIEW_WIDTH, 0.0f, float(getHeight()));
}

void Polytempo_PageEditorView::resized()
{
    Rectangle<int> r (getLocalBounds());
    
    tree->setBounds(r.withWidth(TREE_VIEW_WIDTH));
    pageEditorViewport->setBounds(r.withLeft(TREE_VIEW_WIDTH+1).withTrimmedRight(TREE_VIEW_WIDTH));
        
    imageFileLabel->setBounds(getWidth() - TREE_VIEW_WIDTH + 5, 0, TREE_VIEW_WIDTH - 20, 34);
    imageFileTextbox->setBounds(getWidth() - TREE_VIEW_WIDTH + 10, 50, TREE_VIEW_WIDTH - 20, 26);
    chooseImageFile->setBounds(getWidth() - TREE_VIEW_WIDTH + 10, 80, 50, 16);
    
    relativePositionLabel->setBounds(getWidth() - TREE_VIEW_WIDTH + 5, 0, TREE_VIEW_WIDTH - 20, 34);
    
    xTextbox->setBounds(getWidth() - TREE_VIEW_WIDTH + 10, 50, 55, 26);
    yTextbox->setBounds((int)(getWidth() - TREE_VIEW_WIDTH * 0.5 + 10), 50, 55, 26);
    wTextbox->setBounds(getWidth() - TREE_VIEW_WIDTH + 10, 95, 55, 26);
    hTextbox->setBounds((int)(getWidth() - TREE_VIEW_WIDTH * 0.5 + 10), 95, 55, 26);
    
    sectionInstancesLabel->setBounds(getWidth() - TREE_VIEW_WIDTH + 5, 140, TREE_VIEW_WIDTH - 20, 34);
    sectionInstancesViewport->setBounds(r.withLeft(int(getWidth()) - TREE_VIEW_WIDTH + 1).withTrimmedTop(170));
}

//------------------------------------------------------------------------------
#pragma mark -
#pragma mark actions

#if defined JUCE_ANDROID || defined JUCE_IOS
void Polytempo_PageEditorView::mouseDoubleClick(const MouseEvent &event)
{
    showMenu();
}
#else
void Polytempo_PageEditorView::mouseDown(const MouseEvent &event)
{
    if (event.mods.isPopupMenu())
    {
        showMenu();
    }
}
#endif

void Polytempo_PageEditorView::showMenu()
{
    ApplicationCommandManager *commandManager = Polytempo_NetworkApplication::getCommandManager();
    PopupMenu m;

    m.addCommandItem(commandManager, Polytempo_CommandIDs::loadImage);
    m.addCommandItem(commandManager, Polytempo_CommandIDs::addSection);
    m.addCommandItem(commandManager, Polytempo_CommandIDs::addInstance);
    m.addSeparator();
    m.addCommandItem(commandManager, Polytempo_CommandIDs::deleteSelected);

    m.showMenuAsync(PopupMenu::Options(), nullptr);
}

static void saveOkCancelCallback(int result, Polytempo_PageEditorView* pParent)
{
	if (result)
	{
		pParent->performDeleteSelected();
	}
}

void Polytempo_PageEditorView::deleteSelected()
{
	imageID = selectedItem->getProperty("imageID");
	String fileName = Polytempo_ImageManager::getInstance()->getFileName(imageID);
	sectionID = selectedItem->getProperty("sectionID");

	String text("Do you want to ");
	if (sectionID == var()) text << "remove \"Image " << imageID.toString() << "\" (" << fileName << ")?";
	else                       text << "delete \"Section " << sectionID.toString() << "\"?";

	Polytempo_OkCancelAlert::show("Delete?", text, ModalCallbackFunction::create(saveOkCancelCallback, this));
}

void Polytempo_PageEditorView::performDeleteSelected()
{
	if (sectionID == var())
	{
		if (Polytempo_ImageManager::getInstance()->deleteImage(imageID))
		{
			score->setDirty();

			// remove the image from the page editor view
			pageEditorViewport->getComponent()->setImage(nullptr);

			// delete the load image event
			for (int i = 0; i < loadImageEvents.size(); i++)
			{
				if (loadImageEvents[i]->getProperty(eventPropertyString_ImageID) == imageID)
				{
					score->removeEvent(loadImageEvents[i], true);
					break;
				}
			}
			// delete all define section events that refer to this image ID
			for (int i = 0; i < addSectionEvents.size(); i++)
			{
				if (addSectionEvents[i]->getProperty(eventPropertyString_ImageID) == imageID)
				{
					var tempSectionID = addSectionEvents[i]->getProperty(eventPropertyString_SectionID);

					// delete all image events (= instances) that refer to this section ID
					for (int j = 0; j < imageEvents.size(); j++)
					{
						if (imageEvents[j]->getProperty(eventPropertyString_SectionID) == tempSectionID)
						{
							score->removeEvent(imageEvents[j]);
						}
					}

					score->removeEvent(addSectionEvents[i], true);
				}
			}
			// delete all image events that refer directly to this image ID
			for (int i = 0; i < imageEvents.size(); i++)
			{
				if (imageEvents[i]->getProperty(eventPropertyString_ImageID) == imageID)
				{
					score->removeEvent(imageEvents[i]);
				}
			}

			selectedItem = nullptr;
		}
	}
	else
	{
		score->setDirty();

		// delete the define section event
		for (int i = 0; i < addSectionEvents.size(); i++)
		{
			if (addSectionEvents[i]->getProperty(eventPropertyString_SectionID) == sectionID)
			{
				score->removeEvent(addSectionEvents[i], true);
				break;
			}
		}

		// delete all image events (= instances) that refer to this section ID
		for (int j = 0; j < imageEvents.size(); j++)
		{
			if (imageEvents[j]->getProperty(eventPropertyString_SectionID) == sectionID)
			{
				score->removeEvent(imageEvents[j]);
			}
		}
	}

	refresh();
}

int Polytempo_PageEditorView::findNewID(String eventPropertyString, Array < Polytempo_Event* > events)
{
    int newID = 0;
    bool success = false;
    
    while(!success)
    {
        newID++;
        success = true;
        for(int i=0;i<events.size();i++)
        {
            if(events[i]->getProperty(eventPropertyString).equals(var(newID)))
            {
                success = false;
                break;
            }
        }
    }
    
    return newID;
}


void Polytempo_PageEditorView::loadImage()
{
    File directory(Polytempo_StoredPreferences::getInstance()->getProps().getValue("scoreFileDirectory"));
    FileChooser fileChooser("Open Image File", directory, "*.jpg;*.png", true);

#ifdef JUCE_ANDROID
    fc.reset(new FileChooser("Open Image File", directory, "*.png;*.jpg", true));
    fc->launchAsync(FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles,
                    [this](const FileChooser &chooser) {
                        File result = chooser.getResult();
                        loadImage(result);
                    });
#else
    if(fileChooser.browseForFileToOpen())
    {
        loadImage(fileChooser.getResult());
    }
#endif
}

void Polytempo_PageEditorView::loadImage(File file)
{
    File directory(Polytempo_StoredPreferences::getInstance()->getProps().getValue("scoreFileDirectory"));
    String url = file.getRelativePathFrom(directory);

    Polytempo_Event *event = Polytempo_Event::makeEvent(eventType_LoadImage);
    event->setProperty(eventPropertyString_URL, url);

    event->setProperty(eventPropertyString_ImageID,
                       findNewID(eventPropertyString_ImageID, loadImageEvents));

    score->addEvent(event, true); // add to init
    score->setDirty();

    Polytempo_EventScheduler::getInstance()->scheduleEvent(event); // execute event

    selectedEvent = event;
    refresh();
}

void Polytempo_PageEditorView::addSection()
{
    Polytempo_Event* event = Polytempo_Event::makeEvent(eventType_AddSection);
    
    event->setProperty(eventPropertyString_ImageID, imageID);
    event->setProperty(eventPropertyString_SectionID, findNewID(eventPropertyString_SectionID, addSectionEvents));
    
    event->setProperty(eventPropertyString_Rect, Polytempo_Event::defaultRectangle());
    
    score->addEvent(event, true);
    score->setDirty();
    
    selectedEvent = event;
    refresh();
}

void Polytempo_PageEditorView::addInstance()
{
    if(selectedImageEvent == nullptr)
    {
        Polytempo_Event* event;
        
        // add a region if there are none
        if(addRegionEvents.size() == 0)
        {
            event = Polytempo_Event::makeEvent(eventType_AddRegion);
            event->setProperty(eventPropertyString_RegionID, "1");
            event->setProperty(eventPropertyString_Rect, Polytempo_Event::defaultRectangle());
            
            score->addEvent(event, true); // add to init
            addRegionEvents.add(event);
        }
        
        event = Polytempo_Event::makeEvent(eventType_Image);
        event->setTime(0);
        event->setProperty(eventPropertyString_SectionID, sectionID);
        event->setProperty(eventPropertyString_RegionID, addRegionEvents[0]->getProperty(eventPropertyString_RegionID));
        
        score->addEvent(event);
        score->setDirty();
        
        refresh();
    }
}

//------------------------------------------------------------------------------
#pragma mark -
#pragma mark retrieve state

bool Polytempo_PageEditorView::hasSelectedImage()
{
    return imageID != var();
}

bool Polytempo_PageEditorView::hasSelectedSection()
{
    return sectionID != var();
}


//------------------------------------------------------------------------------
#pragma mark -
#pragma mark label listener

void Polytempo_PageEditorView::editorShown(Label*, TextEditor&)
{}

void Polytempo_PageEditorView::labelTextChanged(Label* label)
{
    if(label == imageFileTextbox)
    {
        if(Polytempo_ImageManager::getInstance()->replaceImage(imageID, label->getTextValue().toString()))
        {
            score->setDirty();
        }
        refresh();
    }
    else if(label == xTextbox || label == yTextbox || label == wTextbox || label == hTextbox)
    {
        Array<var> r;
        if (selectedAddSectionEvent->hasProperty(eventPropertyString_Rect))
            r = *selectedAddSectionEvent->getProperty(eventPropertyString_Rect).getArray();
        else
            r = Polytempo_Event::defaultRectangle();
        
        pageEditorViewport->getComponent()->setSectionRect(Rectangle<float>(r[0],r[1],r[2],r[3]));
        float num = label->getText().getFloatValue();
        num = num < 0.0f ? 0.0f : num > 1.0f ? 1.0f : num;
        
        if     (label == xTextbox) r.set(0, num);
        else if(label == yTextbox) r.set(1, num);
        else if(label == wTextbox) r.set(2, num);
        else if(label == hTextbox) r.set(3, num);
        
        selectedAddSectionEvent->setProperty(eventPropertyString_Rect, r);
        score->setDirty();
        
        update();
    }
}

//------------------------------------------------------------------------------
#pragma mark -
#pragma mark button listener

void Polytempo_PageEditorView::buttonClicked(Button* button) {
    if (button == chooseImageFile) {
        File directory(Polytempo_StoredPreferences::getInstance()->getProps().getValue(
                "scoreFileDirectory"));

#ifdef JUCE_ANDROID
        fc.reset(new FileChooser("Open Image File", directory, "*.png;*.jpg", true));
        fc->launchAsync(FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles,
                        [this, directory](const FileChooser &chooser) {
                            File result = chooser.getResult();
                            if (Polytempo_ImageManager::getInstance()->replaceImage(
                                    imageID,
                                    result.getRelativePathFrom(directory)))
                            {
                                score->setDirty();
                            }
                            refresh();
                        });
#else
        FileChooser fileChooser ("Open Image File", directory, "*.png;*.jpg");
        if(fileChooser.browseForFileToOpen())
        {
            if(Polytempo_ImageManager::getInstance()->replaceImage(imageID, fileChooser.getResult().getRelativePathFrom(directory)))
            {
                score->setDirty();
            }
            refresh();
        }
#endif
    }
}

//------------------------------------------------------------------------------
#pragma mark -
#pragma mark event observer

void Polytempo_PageEditorView::eventNotification(Polytempo_Event *event)
{
    if(event->getType() == eventType_DeleteAll && isVisible())
    {
        selectedItem = nullptr;
        pageEditorViewport->getComponent()->setImage(nullptr);
        update();
    }
    if(event->getType() == eventType_Ready && isVisible())
    {
        selectedItem = nullptr;
        refresh();
    }
}



