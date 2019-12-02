#pragma once

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
        inputRestriction = String();
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

        textboxLabel = new Label(String(), label);
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
    String inputRestriction = String();
    Label *textboxLabel;
    float numericalRangeMin = 0.0f;
    float numericalRangeMax = 0.0f;
};