#pragma once

#include "../../Scheduler/Polytempo_EventObserver.h"

class Polytempo_Cursor : public Component,
                         public Polytempo_EventObserver
{
public:
    Polytempo_Cursor();
    ~Polytempo_Cursor() override;

    void paint(Graphics& g) override;

    void setTime(int time);
    void setX(float x);
    void setIncrement(float incr);
    void setUsedWidth(float width);

    void eventNotification(Polytempo_Event* event) override;

private:
    int time;
    float increment, elapsedTime;
    float usedWidth;
    OwnedArray<class Rectangle<float>> regionBounds;
};
