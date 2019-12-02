#pragma once

#include "JuceHeader.h"

#define BUTTON_SIZE 35

class Polytempo_GraphicsAnnotationLayer;

class Polytempo_GraphicsPalette : public Button::Listener, Slider::Listener, ChangeListener
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
    float getLastTextSize() const;
    float getLastLineWeight() const;
	void setTemporaryFontSize(float size) const;
    void mouseEntered();
    void mouseLeft();
	
private:
	static Image CreateImageWithSolidBackground(Image image, int targetWidth, int targetHeight);
	void buttonClicked(Button* source) override;
    void sliderValueChanged(Slider *slider) override;
	void changeListenerCallback(ChangeBroadcaster* source) override;
	void buttonStateChanged(Button*) override;
    void setTemporaryColor();
    
private:
	Polytempo_GraphicsAnnotationLayer* pAnnotationLayer;
	std::unique_ptr<ImageButton> buttonOk;
	std::unique_ptr<ImageButton> buttonCancel;
	std::unique_ptr<ImageButton> buttonDelete;
    std::unique_ptr<ColourSelector> colorSelector;
    std::unique_ptr<Slider> transparencySlider;
    std::unique_ptr<Slider> textSizeSlider;
    std::unique_ptr<Slider> lineWeightSlider;
    std::unique_ptr<ImageComponent> transparencyLabel;
    std::unique_ptr<ImageComponent> textSizeLabel;
    std::unique_ptr<ImageComponent> lineWeightLabel;
    
	bool visibleFlag;
	Component* pParentComponent;
	Colour lastColor;
    float lastLineWeight;
    float lastTextSize;

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
