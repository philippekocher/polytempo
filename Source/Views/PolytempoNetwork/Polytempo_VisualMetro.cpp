#include "Polytempo_VisualMetro.h"
#include "Polytempo_NetworkMainView.h"
#include "../../Application/PolytempoNetwork/Polytempo_NetworkApplication.h"
#include "../../Preferences/Polytempo_StoredPreferences.h"

class Polytempo_VisualConductorComponent : public Component
{
public:
    Polytempo_VisualConductorComponent()
    {
        setOpaque(true);
        foregroundColour = Colour::fromString(Polytempo_StoredPreferences::getInstance()->getProps().getValue("stripNormalColour"));
        backgroundColour = Colour::fromString(Polytempo_StoredPreferences::getInstance()->getProps().getValue("stripBackgroundColour"));
        frameColour = Colour::fromString(Polytempo_StoredPreferences::getInstance()->getProps().getValue("stripFrameColour"));

        position = 0;
        subdivisionPosition = 0;
    }

    ~Polytempo_VisualConductorComponent() override
    {
    }

    void paint(Graphics& g) override
    {
        int width = getWidth();
        int height = getHeight();

        if (width > height) isHorizontal = true;
        else isHorizontal = false;

        /* draw the background and frame
         ----------------------------------- */

        g.fillAll(backgroundColour);

        g.setColour(frameColour);
        g.drawHorizontalLine(getHeight() - 1, 0.0f, (float)getWidth());
        g.drawVerticalLine(getWidth() - 1, 0.0f, (float)getHeight());

        /* draw the bar
         ----------------------------------- */

        // don't draw when bar is idle
        if (position <= 0.0f) return;

        g.setImageResamplingQuality(Graphics::lowResamplingQuality);
        // lo quality seems to be ok
        g.setColour(foregroundColour);

        if (isHorizontal) g.fillRect(0.0, 0.0, width * position, height * subdivisionPosition);
        else g.fillRect(0.0, 0.0, width * subdivisionPosition, height * position);
    }

    void setPosition(float pos, float subPos)
    {
        position = pos;
        subdivisionPosition = subPos;
    }

    void setForegroundColour(Colour& colour)
    {
        foregroundColour = colour;
    }

    void setBackgroundColour(Colour& colour)
    {
        backgroundColour = colour;
    }

    void setFrameColour(Colour& colour)
    {
        frameColour = colour;
    }

private:
    float position, subdivisionPosition;
    bool isHorizontal;
    Colour foregroundColour, backgroundColour, frameColour;
};

Polytempo_VisualMetro::Polytempo_VisualMetro(): Polytempo_EventObserver()
{
    setOpaque(true);
    width = 0.0f;
    pos = 0.5f;
    timeInterval = 20; // update interval in milliseconds (equals 50 fps)
    tempoFactor = 1.0f;
    pattern = 0;
    exponentMain = 1.5f;
    exponentSub = 1.0f;
    linear = false;
    shouldStop = true;

    normalColour = Colour::fromString(Polytempo_StoredPreferences::getInstance()->getProps().getValue("stripNormalColour"));
    cueColour = Colour::fromString(Polytempo_StoredPreferences::getInstance()->getProps().getValue("stripCueColour"));
    foregroundColour = normalColour;

    addAndMakeVisible(hComponent = new Polytempo_VisualConductorComponent());
    addAndMakeVisible(vComponent = new Polytempo_VisualConductorComponent());
}

Polytempo_VisualMetro::~Polytempo_VisualMetro()
{
    deleteAllChildren();
}

void Polytempo_VisualMetro::setForegroundColour(Colour& colour)
{
    hComponent->setForegroundColour(colour);
    vComponent->setForegroundColour(colour);
    foregroundColour = colour;
}

void Polytempo_VisualMetro::setBackgroundColour(Colour& colour)
{
    hComponent->setBackgroundColour(colour);
    vComponent->setBackgroundColour(colour);
}

void Polytempo_VisualMetro::setFrameColour(Colour& colour)
{
    hComponent->setFrameColour(colour);
    vComponent->setFrameColour(colour);
}

void Polytempo_VisualMetro::setWidth(float val)
{
    width = val;

    hComponent->setBounds((int)width, 0, (int)(getWidth() - width), (int)width);
    vComponent->setBounds(0, (int)width, (int)width, (int)(getWidth() - width));

    repaint();
}

void Polytempo_VisualMetro::paint(Graphics& g)
{
    g.setColour(foregroundColour);
    g.fillRect(0, 0, (int)width, (int)width);
}

void Polytempo_VisualMetro::timerCallback()
{
    if (width == 0) return;

    // stop when a new beat event is overdue
    if (pos > 1.1f && !linear)
    {
        shouldStop = true;
        // the bar moves back to 0 with the same speed as the last beat
        // but not slower than a beat of 60 bpm
        float minIncrement = increment = timeInterval / 1000.0f;
        increment = increment < minIncrement ? minIncrement : increment;
    }
    else if (pos > 1.0f && linear)
    {
        x = y = 0;
        stopTimer();

        hComponent->setPosition(x, subpos);
        vComponent->setPosition(y, subpos);
        
        repaint();
        return;
    }

    if (shouldStop && pos > 0.4f && pos < 0.6f)
    {
        pattern = 0;
        stopTimer();
    }

    /* conductor position
     ----------------------------------- */

    float ictus = 1.0f - pow(increment * 5, 2.0f);
    /* This factor creates a sudden jerk at the end of the movement. The animation of the conductor reaches only a certain amount of the total length (ictus < 1.0, dependent on the tempo) and jumps to the max when the next beat is due.
     */
    ictus = ictus < 0 ? 0 : ictus; // must not be negative!

    if (beatSubdivision > 1)
    {
        subpos = (int)(pos * 1000) % (1000 / beatSubdivision) / (float)(1000 / beatSubdivision);
        subpos = 1.0f - 0.75f * pow(subpos, exponentSub);
    }
    else
    {
        subpos = 1;
    }

    if (pattern == 11 || pattern == 1 ||
        (pattern == 10 && pos <= 0.5f) ||
        (pattern == 12 && pos <= 0.5f) ||
        (pattern == 21 && pos >= 0.5f))
    {
        x = 0;
        if (holdMax) y = 1;
        else if (linear) y = fabs(pos * 2 - 1);
        else y = pow(fabs(pos * 2 - 1) * ictus, exponentMain);
    }
    else if (pattern == 22 || pattern == 2 ||
        (pattern == 20 && pos <= 0.5f) ||
        (pattern == 21 && pos <= 0.5f) ||
        (pattern == 12 && pos >= 0.5f))
    {
        y = 0;
        if (holdMax) x = 1;
        else if (linear) x = fabs(pos * 2 - 1);
        else x = pow(fabs(pos * 2.0f - 1.0f) * ictus, exponentMain);
    }
    else
    {
        x = y = 0;
    }

    hComponent->setForegroundColour(foregroundColour);
    vComponent->setForegroundColour(foregroundColour);

    hComponent->setPosition(x, subpos);
    vComponent->setPosition(y, subpos);

    // position increment
    if (shouldStop && pos > 0.5) pos -= increment;
    else pos += increment;

    if (holdMax) holdMax--;

    // cue colour changes to normal colour on downbeat
    if (pos > 0.5 && (pattern == 11 || pattern == 21))
    {
        foregroundColour = normalColour;
    }
    
    repaint();
}

void Polytempo_VisualMetro::eventNotification(Polytempo_Event* event)
{
    if (!Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("showVisualMetro")) return;

    if (event->getType() == eventType_Beat)
    {
        if (event->hasProperty(eventPropertyString_Duration) && (float)event->getProperty(eventPropertyString_Duration) > 0.0f)
            beatDuration = event->getProperty(eventPropertyString_Duration);
        else
            beatDuration = 1.0f / timeInterval;

        // the subdivision feature is not implemented yet
        beatSubdivision = 0;

        if (event->hasProperty(eventPropertyString_Pattern))
            pattern = event->getProperty(eventPropertyString_Pattern);
        else
            pattern = eventPropertyDefault_Pattern;

        if (int(event->getProperty(eventPropertyString_Cue)) == 1)
            foregroundColour = cueColour;
        else
            foregroundColour = normalColour;

        if (int(event->getProperty(eventPropertyString_Linear)) != 0)
            linear = true;
        else
            linear = false;

        shouldStop = false;

        /* the look and feel of the conductor's movement is based on three parameters: (1) an exponent that depends on the duration of the beat, (2) a certain point at which the conductor jumps immediately to the max, creating an ictus - see above - and (3) a certain duration through which the max is held.  */

        float actualDuration = beatDuration / tempoFactor;
        exponentMain = 0.75f + actualDuration;
        exponentMain = exponentMain > 2.5f ? 2.5f : exponentMain; // clip
        exponentMain = exponentMain < 1.0f ? 1.0f : exponentMain;

        if (pattern < 3) holdMax = 0;
        else holdMax = 50 / timeInterval;

        increment = (float)timeInterval / actualDuration * 0.001f;

        if (pattern < 3 || pattern == 10 || pattern == 20)
            increment *= 0.5f; // only half the distance

        if (pattern > 2) pos = 0.0f;
        else pos = 0.5f;

        startTimer(timeInterval);
    }
    else if (event->getType() == eventType_TempoFactor)
    {
        tempoFactor = event->getProperty("value");
        increment = (float)timeInterval / beatDuration * tempoFactor * 0.001f;
    }
    else if (event->getType() == eventType_Pause)
    {
        shouldStop = true;
    }
    else if (event->getType() == eventType_Stop)
    {
        if (isTimerRunning())
        {
            shouldStop = true;
        }
    }
}
