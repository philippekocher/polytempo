/*
  ==============================================================================

    Polytempo_GraphicsPalette.h
    Created: 16 Jul 2018 8:56:42am
    Author:  christian.schweizer

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#define BUTTON_SIZE 35

class Polytempo_GraphicsAnnotationLayer;

//==============================================================================
/*
*/
class Polytempo_GraphicsPalette : public Button::Listener, ChangeListener
{
private:
	Polytempo_GraphicsPalette();
	~Polytempo_GraphicsPalette();

public:
	juce_DeclareSingleton(Polytempo_GraphicsPalette, false)

	void initialize(Component* pParentComponent);
	void setAnnotationLayer(Polytempo_GraphicsAnnotationLayer* pParent);
	void setVisible(bool show);
	int isVisible() const;
	int resize(Point<int> offset) const;
	Colour getLastColour() const;
	void hitBtnColor();
	void hitBtnTextSize();
	void setTemporaryFontSize(float size) const;
	
private:
	static Image CreateImageWithSolidBackground(Image image, int targetWidth, int targetHeight);
	PopupMenu getTextSizePopupMenu() const;
	void AddFontSizeToMenu(PopupMenu* m, int fontSize) const;
	void buttonClicked(Button* source) override;
	void changeListenerCallback(ChangeBroadcaster* source) override;
	void buttonStateChanged(Button*) override;

private:
	Polytempo_GraphicsAnnotationLayer* pAnnotationLayer;
	std::unique_ptr<ImageButton> buttonOk;
	std::unique_ptr<ImageButton> buttonCancel;
	std::unique_ptr<ImageButton> buttonColor;
	std::unique_ptr<ImageButton> buttonTextSize;
	std::unique_ptr<ImageButton> buttonDelete;
	bool visibleFlag;
	Component* pParentComponent;
	Colour lastColor;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_GraphicsPalette)
};

class FontSizeCallback : public ModalComponentManager::Callback
{
public:
	FontSizeCallback(Polytempo_GraphicsPalette* pParent) :pParent(pParent) {};
	void modalStateFinished(int returnValue) override { if (returnValue > 0) { pParent->setTemporaryFontSize(float(returnValue)); } }

private:
	Polytempo_GraphicsPalette * pParent;
};
