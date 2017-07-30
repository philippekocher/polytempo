/*
  ==============================================================================

    Polytempo_GraphicsAnnotationManagerView.h
    Created: 28 Jul 2017 4:27:03pm
    Author:  chris

  ==============================================================================
*/

#pragma once

#include "../../../POLYTEMPO NETWORK/JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class Polytempo_GraphicsAnnotationManagerView    : public Component, ButtonListener
{
public:
    Polytempo_GraphicsAnnotationManagerView();
    ~Polytempo_GraphicsAnnotationManagerView();

    void paint (Graphics&) override;
    void resized() override;

	void buttonClicked(Button*source) override;

private:
	ScopedPointer<TextButton> btnEdit;
	ScopedPointer<TextButton> btnLoad;
	ScopedPointer<TextButton> btnSave;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_GraphicsAnnotationManagerView)	
};
