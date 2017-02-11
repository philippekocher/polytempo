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


#ifndef __Polytempo_Textbox__
#define __Polytempo_Textbox__

//#include "../JuceLibraryCode/JuceHeader.h"

class Polytempo_Textbox : public Label
{
public:
    static String timeToString(float seconds)
    {
        int miliseconds = (int)(seconds * 1000.0 + 0.5);
        
        int hours = abs(miliseconds) / 3600000;
        int mins  = (abs(miliseconds) / 60000) % 60;
        int secs  = (abs(miliseconds) / 1000) % 60;
        int msecs = abs(miliseconds) % 1000;
        
        String timeString;
        if(miliseconds < 0) timeString << "-";
        timeString << String(hours).paddedLeft('0',2) << ":" << String(mins).paddedLeft('0',2) << ":" << String(secs).paddedLeft('0',2) << "." << String(msecs).paddedLeft('0', 3);
        
        return timeString;
    }
    
    static float stringToTime(String timeString)
    {
        StringArray tokens;
        tokens.addTokens(timeString, ":", "");
        tokens.removeEmptyStrings();
        tokens.trim();
        
        if(tokens.size() == 2)
        {
            return tokens[0].getIntValue() * 60 + tokens[1].getFloatValue();
        }
        else if(tokens.size() == 3)
        {
            return tokens[0].getIntValue() * 3600 + tokens[1].getIntValue() * 60 + tokens[2].getFloatValue();
        }
        
        return timeString.getFloatValue();
    }
    
    
    enum textboxType
    {
        textboxType_normal = 0,
        textboxType_black
    };
    
    Polytempo_Textbox(const String& label, textboxType type = textboxType_normal)
    {
        setJustificationType(Justification::centredLeft);
        setEditable(true, true, false);
        setMinimumHorizontalScale(0.1f);
        reset();
        
        switch(type)
        {
            case textboxType_normal:
                setColour(Label::backgroundColourId, Colour(235,235,235));
                setColour(Label::textColourId, Colour(100,100,100));
                setColour(TextEditor::backgroundColourId, Colours::white);
                setColour(TextEditor::textColourId, Colours::black);
                break;
    
            case textboxType_black:
                setColour(Label::backgroundColourId, Colours::black);
                setColour(Label::textColourId, Colours::white);
                setColour(TextEditor::backgroundColourId, Colours::white);
                setColour(TextEditor::textColourId, Colours::black);
                break;
        }

        textboxLabel = new Label(String::empty, label);
        textboxLabel->setFont(Font (12.0000f, Font::plain));
        textboxLabel->setJustificationType(Justification::bottomLeft);
        textboxLabel->setBorderSize(BorderSize<int>(1));
        textboxLabel->attachToComponent(this, false);
    }
    
    void editorShown(TextEditor* textEditor)
    {
        Point<int> point = textEditor->getMouseXYRelative();
        int index = textEditor->getTextIndexAt(point.x, point.y);
		textEditor->setHighlightedRegion(Range<int>::emptyRange(index));
		textEditor->setInputRestrictions(0,inputRestriction);
    }
    
    void textWasEdited()
    {
        if(numericalRangeMin == 0 && numericalRangeMax == 0)   return;
        else if(getText().getFloatValue() < numericalRangeMin) setText(String(numericalRangeMin), dontSendNotification);
        else if(getText().getFloatValue() > numericalRangeMax) setText(String(numericalRangeMax), dontSendNotification);
    }
    
    void enablementChanged()
    {
        if(isEnabled())
        {
            textboxLabel->setColour(Label::textColourId, Colour(0,0,0));
            setColour(Label::backgroundColourId, Colour(235,235,235));
        }
        else
        {
            textboxLabel->setColour(Label::textColourId, Colour(200,200,200));
            setColour(Label::backgroundColourId, Colour(245,245,245));
        }
    }
    
    void setInputRestrictions(int, String s, float min = 0, float max = 0)
    {
        inputRestriction = s;

        numericalRangeMin = min;
        numericalRangeMax = max;
    }
    
    void setFloat(float num, const NotificationType notification)
    {
        setText(String(num, 0), notification);
    }
 
    void reset()
    {
        setText("--", dontSendNotification);
    }
    
private:
    String inputRestriction = String::empty;
    Label *textboxLabel;
    float numericalRangeMin;
    float numericalRangeMax;
};



#endif  // __Polytempo_Textbox__
