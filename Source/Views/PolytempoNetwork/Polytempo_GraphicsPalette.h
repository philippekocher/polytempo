/*
  ==============================================================================

    Polytempo_GraphicsPalette.h
    Created: 16 Jul 2018 8:56:42am
    Author:  chris

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#define BUTTON_SIZE 32

class Polytempo_GraphicsAnnotationLayer;

//==============================================================================
/*
*/
class Polytempo_GraphicsPalette : public Button::Listener, ChangeListener
{
public:
    Polytempo_GraphicsPalette(Polytempo_GraphicsAnnotationLayer* pParent);
    ~Polytempo_GraphicsPalette();
	void show(bool show) const;
	Colour getCurrentColour() const;
	void hitBtnColor();
	void hitBtnTextSize();
	void setTemporaryFontSize(float size) const;

private:
	static Image CreateImageWithSolidBackground(Image image, int targetWidth, int targetHeight);
	PopupMenu getTextSizePopupMenu() const;
	void AddFontSizeToMenu(PopupMenu* m, int fontSize) const;
	void buttonClicked(Button* source) override;
	void changeListenerCallback(ChangeBroadcaster* source) override;

private:
	Polytempo_GraphicsAnnotationLayer* pParent;
	ScopedPointer<ImageButton> buttonOk;
	ScopedPointer<ImageButton> buttonCancel;
	ScopedPointer<ImageButton> buttonColor;
	ScopedPointer<ImageButton> buttonTextSize;
	ScopedPointer<ImageButton> buttonSettings;
	ScopedPointer<ImageButton> buttonDelete;
	ScopedPointer<ColourSelector> colorSelector;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_GraphicsPalette)
};

class FontSizeCallback : public ModalComponentManager::Callback
{
public:
	FontSizeCallback(Polytempo_GraphicsPalette* pParent) :pParent(pParent) {};
	void modalStateFinished(int returnValue) override { pParent->setTemporaryFontSize(float(returnValue)); }

private:
	Polytempo_GraphicsPalette * pParent;
};
