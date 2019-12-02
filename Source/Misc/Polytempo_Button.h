#ifndef __Polytempo_Button__
#define __Polytempo_Button__


class Polytempo_Button : public TextButton
{
public:
    enum buttonType
    {
        buttonType_normal = 0,
        buttonType_toggle,
        buttonType_toggleYellow,
        buttonType_black,
        buttonType_play,
    };

    Polytempo_Button(const String& label, buttonType type = buttonType_normal)
    {
        setButtonText(label);
        setMouseClickGrabsKeyboardFocus(false);
        setWantsKeyboardFocus(false);
        
        switch(type)
        {
            case buttonType_normal:
                setColour(buttonColourId,   Colour(215,215,215));
                setColour(textColourOffId,  Colour(0,0,0));
                setColour(buttonOnColourId, Colour(135,135,135));
                setColour(textColourOnId,   Colour(255,255,255));
                break;
                
            case buttonType_toggle:
                setClickingTogglesState(true);
                setColour(buttonColourId,   Colour(255,255,255));
                setColour(textColourOffId,  Colour(150,150,150));
                setColour(buttonOnColourId, Colour(215,215,215));
                setColour(textColourOnId,   Colour(0,0,0));
                break;
                
            case buttonType_toggleYellow:
                setClickingTogglesState(true);
                setColour(buttonColourId,   Colour(215,215,215));
                setColour(textColourOffId,  Colour(0,0,0));
                setColour(buttonOnColourId, Colour(215,205,40));
                setColour(textColourOnId,   Colour(255,255,205));
                break;
                
            case buttonType_black:
                setColour(buttonColourId,  Colour(0xff000000));
                setColour(textColourOffId, Colour(0xffffffff));
                break;

            case buttonType_play:
                setColour(buttonColourId,   Colour(215,215,215));
                setColour(textColourOffId,  Colour(0,0,0));
                setColour(buttonOnColourId, Colour(50,255,50));
                setColour(textColourOnId,   Colour(0,50,0));
                break;
                
        }
    }
};

#endif  // __Polytempo_Button__
