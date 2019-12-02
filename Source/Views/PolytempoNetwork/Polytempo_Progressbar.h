#ifndef __Polytempo_Progressbar__
#define __Polytempo_Progressbar__

#include "../../Scheduler/Polytempo_EventObserver.h"

class Polytempo_Progressbar : public Component,
                              public Polytempo_EventObserver
{
public:
    Polytempo_Progressbar();
    ~Polytempo_Progressbar();
    
    void paint(Graphics& g);
    
    void setText(String text);
    void setTime(int time);
    void setDuration(float duration);
    
    void eventNotification(Polytempo_Event *event);

    
private:
    std::unique_ptr < String > text;
    int   time;
    float duration, elapsedTime;
    OwnedArray < class Rectangle<float> > regionBounds;
};



#endif  // __Polytempo_Progressbar__
