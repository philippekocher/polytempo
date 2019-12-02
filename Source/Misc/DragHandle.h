#ifndef __DragHandle__
#define __DragHandle__

class DragHandle;

class DragHandleListener
{
public:
    virtual ~DragHandleListener() {};
    virtual void draggingSessionEnded() = 0;
    virtual void positionChanged(DragHandle*) = 0;
};

class DragHandle : public Component
{
public:
    DragHandle(DragHandleListener* theListener)
    : listener(theListener)
    {
        setSize (15, 15);
        setRepaintsOnMouseActivity (true);
    }
    
    void paint(Graphics& g) override
    {
        g.setColour(Colours::white);
        g.fillRect(getLocalBounds().reduced (3).toFloat());
    
        g.setColour(Colours::black);
        g.drawRect(getLocalBounds().reduced (3).toFloat(), isMouseOverOrDragging() ? 1.5f : 1.0f);
    }
    
    void setConstraint(int c)
    {
        constraint = c;
    }
    
    void setBoundsConstraint(Rectangle < int > r)
    {
        boundsConstraint = r;
    }
    
    void mouseDown(const MouseEvent&) override
    {
        originalPosition = Point<float>(getCentreX(), getCentreY());
    }

    void mouseDrag(const MouseEvent& e) override
    {
        Point <int> point = e.getEventRelativeTo(getParentComponent()).getPosition();
        
        if(boundsConstraint.getWidth() != 0 && boundsConstraint.getHeight() != 0)
        {
            point = boundsConstraint.getConstrainedPoint(point);
        }
        
        if(constraint == 1)
        {
            setCentrePosition(point.x, (int)getCentreY());
        }
        else if(constraint == 2)
        {
            setCentrePosition((int)getCentreX(), point.y);
        }
        else
        {
            setCentrePosition(point.x, point.y);
        }
        
        if(listener) listener->positionChanged(this);
    }
    
    void mouseUp(const MouseEvent&) override
    {
        if(listener) listener->draggingSessionEnded();
    }
    
    float getCentreX()
    {
        return getX() + getWidth() / 2.0f;
    }
    
    float getCentreY()
    {
        return getY() + getHeight() / 2.0f;
    }
    
private:
    
    DragHandleListener* listener;
    Point<float> originalPosition;
    Rectangle<int> boundsConstraint;
    int constraint = 0; // 0=no constraint, 1=horizontal only, 2=vertical only
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DragHandle)
};

#endif  // __Handle__
