#pragma once

#include "../../Scheduler/Polytempo_EventObserver.h"

class Polytempo_Progressbar : public Component,
                              public Polytempo_EventObserver
{
public:
    Polytempo_Progressbar();
    ~Polytempo_Progressbar() override;

    void paint(Graphics& g) override;

    void setText(String text);
    void setTime(int time);
    void setDuration(float duration);

    void eventNotification(Polytempo_Event* event) override;

private:
    std::unique_ptr<String> text;
    int time;
    float duration, elapsedTime;
    OwnedArray<class Rectangle<float>> regionBounds;
};
