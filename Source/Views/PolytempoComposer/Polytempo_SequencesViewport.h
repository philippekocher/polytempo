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

#ifndef __Polytempo_SequencesViewport__
#define __Polytempo_SequencesViewport__


#include "../JuceLibraryCode/JuceHeader.h"
#include "../../Data/Polytempo_Composition.h"
#include "../../Misc/Polytempo_Button.h"


class Polytempo_ColourButton : public Button, ChangeListener
{
public:
    Polytempo_ColourButton(int index) : Button(String()), sequenceIndex(index)
    {}
    
    void paintButton(Graphics &g, bool /*mouseOver*/, bool /*buttonDown*/) override
    {
        Polytempo_Composition* composition = Polytempo_Composition::getInstance();
        
        Colour colour = composition->getSequence(sequenceIndex)->getColour();
        g.fillAll(colour);
    }
    
    void clicked() override
    {
        ColourSelector* colourSelector = new ColourSelector(ColourSelector::showColourspace);
        colourSelector->setName("background");
        colourSelector->setCurrentColour(Polytempo_Composition::getInstance()->getSequence(sequenceIndex)->getColour());
        colourSelector->addChangeListener(this);
        colourSelector->setColour(ColourSelector::backgroundColourId, Colours::transparentBlack);
        colourSelector->setSize(200, 200);
        
        CallOutBox::launchAsynchronously(colourSelector, getScreenBounds(), nullptr);
    }
    
    void changeListenerCallback (ChangeBroadcaster* source) override
    {
        if (ColourSelector* cs = dynamic_cast<ColourSelector*> (source))
            Polytempo_Composition::getInstance()->getSequence(sequenceIndex)->setColour(cs->getCurrentColour());

        Polytempo_Composition::getInstance()->updateContent(); // repaint everything
    }
    
private:
    int sequenceIndex;
};


class Polytempo_SequenceControlComponent : public Component,
                                           public TextEditor::Listener,
                                           public Button::Listener
{
public:
    Polytempo_SequenceControlComponent(int);
    ~Polytempo_SequenceControlComponent();

    void paint (Graphics&);
    void resized();
    
    void mouseDown(const MouseEvent &);
    
    void textEditorTextChanged(TextEditor&);
    void buttonClicked(Button*);

private:
    int sequenceIndex;
    TextEditor sequenceName;
    
    ScopedPointer < Polytempo_Button > settingsButton;
    ScopedPointer < Polytempo_Button > soloButton;
    ScopedPointer < Polytempo_Button > muteButton;

    ScopedPointer<Polytempo_ColourButton> colourSelector;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_SequenceControlComponent)
};


class Polytempo_SequencesViewport : public Viewport
{
public:
    Polytempo_SequencesViewport();
    ~Polytempo_SequencesViewport();

    void paint (Graphics&);
    void resized();

private:
    ScopedPointer <Component> viewedComponent;
    OwnedArray <Polytempo_SequenceControlComponent> sequenceControls;
};

class Polytempo_SequencesComponent : public Component
{
public:
    void mouseDown(const MouseEvent &);
    void showPopupMenu();

};


#endif  // __Polytempo_SequencesViewport__
