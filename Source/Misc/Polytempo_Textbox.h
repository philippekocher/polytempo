#pragma once

class Polytempo_Textbox : public Label
{
public:
    static String timeToString(float seconds, bool displayMilliseconds = true)
    {
        int miliseconds = (int)(seconds * 1000.0 + 0.5);

        int hours = abs(miliseconds) / 3600000;
        int mins = (abs(miliseconds) / 60000) % 60;
        int secs = (abs(miliseconds) / 1000) % 60;
        int msecs = abs(miliseconds) % 1000;

        String timeString;
        if (miliseconds < 0) timeString << "-";
        timeString << String(hours).paddedLeft('0', 2) << ":" << String(mins).paddedLeft('0', 2) << ":" << String(secs).paddedLeft('0', 2);
        
        if (displayMilliseconds)
            timeString = timeString << "." << String(msecs).paddedLeft('0', 3);

        return timeString;
    }

    static float stringToTime(String timeString)
    {
        StringArray tokens;
        tokens.addTokens(timeString, ":", "");
        tokens.removeEmptyStrings();
        tokens.trim();

        if (tokens.size() == 2)
        {
            return tokens[0].getIntValue() * 60 + tokens[1].getFloatValue();
        }

        if (tokens.size() == 3)
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

        switch (type)
        {
        case textboxType_normal:
            setColour(backgroundColourId, Colour(235, 235, 235));
            setColour(textColourId, Colour(100, 100, 100));
            setColour(TextEditor::backgroundColourId, Colours::white);
            setColour(TextEditor::textColourId, Colours::black);
            break;

        case textboxType_black:
            setColour(backgroundColourId, Colours::black);
            setColour(textColourId, Colours::white);
            setColour(TextEditor::backgroundColourId, Colours::white);
            setColour(TextEditor::textColourId, Colours::black);
            break;
        }

        textboxLabel = new Label(String(), label);
        textboxLabel->setFont(Font(12.0000f, Font::plain));
        textboxLabel->setJustificationType(Justification::bottomLeft);
        textboxLabel->setBorderSize(BorderSize<int>(1));
        textboxLabel->attachToComponent(this, false);
    }

    void editorShown(TextEditor* textEditor)
    {
        Point<int> point = textEditor->getMouseXYRelative();
        int index = textEditor->getTextIndexAt(point.x, point.y);
        textEditor->setHighlightedRegion(Range<int>::emptyRange(index));
        textEditor->setInputRestrictions(0, inputRestriction);
    }

    void textWasEdited()
    {
        if (numericalRangeMin == 0 && numericalRangeMax == 0)
            return;

        if (getText().getFloatValue() < numericalRangeMin)
            setText(String(numericalRangeMin), dontSendNotification);

        else if (getText().getFloatValue() > numericalRangeMax)
            setText(String(numericalRangeMax), dontSendNotification);
    }

    void enablementChanged()
    {
        if (isEnabled())
        {
            textboxLabel->setColour(textColourId, Colour(0, 0, 0));
            setColour(backgroundColourId, Colour(235, 235, 235));
        }
        else
        {
            textboxLabel->setColour(textColourId, Colour(200, 200, 200));
            setColour(backgroundColourId, Colour(245, 245, 245));
        }
    }

    void setInputRestrictions(int, String s, float min = 0, float max = 0)
    {
        inputRestriction = s;

        numericalRangeMin = min;
        numericalRangeMax = max;
    }

    void setFloat(float num, const NotificationType /*notification*/)
    {
        set(String(num, 0));
    }
    
    void set(const String& newText)
    {
        MessageManager::callAsync([this, newText]() {
            setText(newText, dontSendNotification);
        });
    }

    void reset()
    {
        setText("--", dontSendNotification);
    }

private:
    String inputRestriction = String();
    Label* textboxLabel;
    float numericalRangeMin = 0.0f;
    float numericalRangeMax = 0.0f;
};
