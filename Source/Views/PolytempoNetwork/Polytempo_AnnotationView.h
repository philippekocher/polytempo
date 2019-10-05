/*
  ==============================================================================

    Polytempo_AnnotationView.h
    Created: 14 Sep 2019 5:19:28pm
    Author:  Christian Schweizer

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"


class Polytempo_AnnotationView : public Component, Button::Listener, ComboBox::Listener, ChangeListener
{
public:
	Polytempo_AnnotationView();
	~Polytempo_AnnotationView();

	void paint(Graphics&) override;
	void resized() override;
	void updateState() const;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Polytempo_AnnotationView)
		
	ToggleButton	*annotationMasterToggle;
	ComboBox		*annotationModeComboBox;
	ImageButton		*buttonSettings;

	void buttonClicked(Button*) override;
	void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override;
	void changeListenerCallback(ChangeBroadcaster* source) override;
    void mouseEnter(const MouseEvent &event) override;
    void mouseExit(const MouseEvent &event) override;
	void displayMode() const;
};
