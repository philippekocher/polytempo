#include "Polytempo_AnnotationView.h"
#include "Polytempo_GraphicsAnnotationManager.h"
#include "Polytempo_GraphicsPalette.h"

Polytempo_AnnotationView::Polytempo_AnnotationView()
{
	Image imageSettings = ImageCache::getFromMemory(BinaryData::settings_png, BinaryData::settings_pngSize);

	addAndMakeVisible(annotationModeComboBox = new ComboBox());
	annotationModeComboBox->addItem("Read only", Polytempo_GraphicsAnnotationManager::ReadOnly);
	annotationModeComboBox->addItem("Standard", Polytempo_GraphicsAnnotationManager::Standard);
	annotationModeComboBox->addItem("Edit mode", Polytempo_GraphicsAnnotationManager::Edit);
	annotationModeComboBox->addListener(this);
	annotationModeComboBox->setWantsKeyboardFocus(false);

	addAndMakeVisible(annotationMasterToggle = new ToggleButton("Annotations"));
	annotationMasterToggle->addListener(this);
	annotationMasterToggle->setWantsKeyboardFocus(false);

	addAndMakeVisible(buttonSettings = new ImageButton("Settings"));
	buttonSettings->setImages(false, false, false, imageSettings, 1.0f, Colours::transparentWhite, Image(), 1.0, Colour(Colours::purple.getRed(), Colours::purple.getGreen(), Colours::purple.getBlue(), uint8(80)) , Image(), 1.0, Colours::green);
	buttonSettings->setBounds(0, 0, BUTTON_SIZE, BUTTON_SIZE);
	buttonSettings->addListener(this);
	displayMode();

    addMouseListener(this, true);
    
	Polytempo_GraphicsAnnotationManager::getInstance()->addChangeListener(this);

	Polytempo_GraphicsPalette::getInstance()->initialize(this);
}

Polytempo_AnnotationView::~Polytempo_AnnotationView()
{
	deleteAllChildren();
}

void Polytempo_AnnotationView::paint(Graphics&)
{
}

void Polytempo_AnnotationView::resized()
{
	annotationMasterToggle->setBounds(0, 5, getWidth(), 18);
	annotationModeComboBox->setBounds(0, 24, getWidth()-30, 22);
	buttonSettings->setBounds(getWidth() - 22, 24, 22, 22);

	int paletteHeight = Polytempo_GraphicsPalette::getInstance()->resize(Point<int>(0, 60));
	setSize(getWidth(), 60 + paletteHeight);
	getParentComponent()->resized();
}

void Polytempo_AnnotationView::updateState() const
{
	annotationModeComboBox->setEnabled(annotationMasterToggle->getToggleState());
	buttonSettings->setEnabled(annotationMasterToggle->getToggleState());
	if(annotationMasterToggle->getToggleState())
	{
		Polytempo_GraphicsAnnotationManager::getInstance()->setAnnotationMode(Polytempo_GraphicsAnnotationManager::eAnnotationMode(annotationModeComboBox->getSelectedId()));
	}
	else
	{
		Polytempo_GraphicsAnnotationManager::getInstance()->setAnnotationMode(Polytempo_GraphicsAnnotationManager::Off);
	}
}

void Polytempo_AnnotationView::buttonClicked(Button* btn)
{
	if(btn == annotationMasterToggle)
	{
		updateState();
	}
	else if(btn == buttonSettings)
	{
		Polytempo_GraphicsAnnotationManager::getInstance()->showSettingsDialog();
	}
}

void Polytempo_AnnotationView::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
	if(comboBoxThatHasChanged == annotationModeComboBox)
	{
		updateState();
	}
}

void Polytempo_AnnotationView::changeListenerCallback(ChangeBroadcaster* /*source*/)
{
	displayMode();
}

void Polytempo_AnnotationView::mouseEnter(const MouseEvent &)
{
    Polytempo_GraphicsPalette::getInstance()->mouseEntered();
}

void Polytempo_AnnotationView::mouseExit(const MouseEvent &)
{
    Polytempo_GraphicsPalette::getInstance()->mouseLeft();
}

void Polytempo_AnnotationView::displayMode() const
{
	Polytempo_GraphicsAnnotationManager::eAnnotationMode mode = Polytempo_GraphicsAnnotationManager::getInstance()->getAnnotationMode();
	annotationModeComboBox->setSelectedId(mode == Polytempo_GraphicsAnnotationManager::Off ? Polytempo_GraphicsAnnotationManager::eAnnotationMode::ReadOnly : mode, sendNotification);
	annotationMasterToggle->setToggleState(mode != Polytempo_GraphicsAnnotationManager::Off, sendNotification);
}
