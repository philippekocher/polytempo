#pragma once

class Polytempo_ImageButton : public DrawableButton
{
public:
    Polytempo_ImageButton() : DrawableButton("xxx", DrawableButton::ButtonStyle::ImageOnButtonBackground)
    {}
    
    Rectangle<float> getImageBounds() const
    {
        auto r = getLocalBounds();
        r = r.reduced(3, 3);
        return r.toFloat();
    }
};
