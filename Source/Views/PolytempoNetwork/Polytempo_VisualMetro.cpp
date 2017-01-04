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
    ~Polytempo_VisualConductorComponent() {}
    
    void paint (Graphics& g) 
    {
        int width = getWidth();
        int height = getHeight();
        
        if(width > height) isHorizontal = true;
        else               isHorizontal = false;
        
       
        /* draw the background and frame
         ----------------------------------- */

        g.fillAll (backgroundColour);
        
        g.setColour(frameColour);
        g.drawHorizontalLine(getHeight() - 1, 0, getWidth());
        g.drawVerticalLine(getWidth() - 1, 0, getHeight());


        /* draw the bar
         ----------------------------------- */
        
        // don't draw when bar is idle
        if(position == 0.0) return;

        g.setImageResamplingQuality(Graphics::lowResamplingQuality);
        // lo quality seems to be ok
        g.setColour(foregroundColour);

        if(isHorizontal)    g.fillRect(0.0, 0.0, width * position, height * subdivisionPosition);
        else                g.fillRect(0.0, 0.0, width * subdivisionPosition, height * position);
    }
    
    void setPosition(float pos, float subPos)
    {
        position = pos;
        subdivisionPosition = subPos;
        repaint();
    }

    void setForegroundColour (Colour& colour)
    {
        foregroundColour = colour;
    }
    
    void setBackgroundColour (Colour& colour)
    {
        backgroundColour = colour;
    }
    
    void setFrameColour (Colour& colour)
    {
        frameColour = colour;
    }
    
private:
    float position, subdivisionPosition;
    bool isHorizontal;
    Colour foregroundColour, backgroundColour, frameColour;
};


Polytempo_VisualMetro::Polytempo_VisualMetro()
: Polytempo_EventObserver()
{
    setOpaque(true);
    width = 0;
    pos = 0.5;
    timeInterval = 15; // good somewhere between 5 and 20
    tempoFactor = 1.0;
    pattern = 0;
    exponentMain = 1.5;
    exponentSub = 1.0;
    shouldStop = true;

    normalColour = Colour::fromString(Polytempo_StoredPreferences::getInstance()->getProps().getValue("stripNormalColour"));
    cueColour = Colour::fromString(Polytempo_StoredPreferences::getInstance()->getProps().getValue("stripCueColour"));
    foregroundColour = normalColour;

    addAndMakeVisible (hComponent = new Polytempo_VisualConductorComponent());
    addAndMakeVisible (vComponent = new Polytempo_VisualConductorComponent());
    
    // optimized redraw - better??
    //setBufferedToImage(true);
}

Polytempo_VisualMetro::~Polytempo_VisualMetro()
{
    deleteAllChildren();
}

void Polytempo_VisualMetro::setForegroundColour (Colour& colour)
{
    hComponent->setForegroundColour(colour);
    vComponent->setForegroundColour(colour);
    foregroundColour = colour;
}

void Polytempo_VisualMetro::setBackgroundColour (Colour& colour)
{
    hComponent->setBackgroundColour(colour);
    vComponent->setBackgroundColour(colour);
}

void Polytempo_VisualMetro::setFrameColour (Colour& colour)
{
    hComponent->setFrameColour(colour);
    vComponent->setFrameColour(colour);
}

void Polytempo_VisualMetro::setWidth(float val)
{
    width = val;
    
    hComponent->setBounds(width, 0, getWidth()-width, width);
    vComponent->setBounds(0, width, width, getWidth()-width);

    repaint();
}

void Polytempo_VisualMetro::paint(Graphics& g)
{
    g.setColour(foregroundColour);
    g.fillRect(0, 0, (int)width, (int)width);
}

/* Unlike the normal Timer class, the hiRes Timer uses a dedicated thread, not the message thread */
void Polytempo_VisualMetro::hiResTimerCallback()
{
    if(width == 0) return;
    
    // stop when a new beat event is overdue
    if(pos > 1.1 )
    {
        shouldStop = true;
    }
    
    if(shouldStop && pos > 0.4 && pos < 0.6)
    {
        pos = 0.5;
        stopTimer();
        

        Polytempo_NetworkApplication* const app = dynamic_cast<Polytempo_NetworkApplication*>(JUCEApplication::getInstance());
        if(app->quitApplication) app->applicationShouldQuit();
    }
    
    /* conductor position
     ----------------------------------- */
    
    float ictus = 1.0 - increment * 5;
    /* This factor creates a sudden jerk at the end of the movement. The animation of the conductor reaches only a certain amount of the total length (ictus < 1.0, dependent on the tempo) and jumps to the max when the next beat is due.
     */
    
    if(beatSubdivision > 1)
    {
        subpos = (int)(pos * 1000) % (1000 / beatSubdivision) / (float)(1000 / beatSubdivision);
        subpos = 1 - 0.75 * pow(subpos, exponentSub);
    }
    else
    {
        subpos = 1;
    }

    if(pattern == 11 ||
       (pattern == 10 && pos <= 0.5) ||
       (pattern == 12 && pos <= 0.5) ||
       (pattern == 21 && pos >= 0.5))
    {
        x = 0;
        if(holdMax) y = 1;
        else        y = pow(fabs(pos * 2 - 1) * ictus, exponentMain);
    }
    else if(pattern == 22 ||
       (pattern == 20 && pos <= 0.5) ||
       (pattern == 21 && pos <= 0.5) ||
       (pattern == 12 && pos >= 0.5))
    {
        y = 0;
        if(holdMax) x = 1;
        else        x = pow(fabs(pos * 2 - 1) * ictus, exponentMain);
    }
    else
    {
        x = y = 0;
    }

    // because this is not the main message thread, we mustn't do
    // any UI work without first grabbing a MessageManagerLock.
    const MessageManagerLock mml (Thread::getCurrentThread());

    hComponent->setForegroundColour(foregroundColour);
    vComponent->setForegroundColour(foregroundColour);

    hComponent->setPosition(x,subpos);
    vComponent->setPosition(y,subpos);
    repaint(0, 0, (int)width, (int)width);


    // position increment
    if(shouldStop && pos > 0.5)     pos -= increment;
    else                            pos += increment;
    
    if(holdMax) holdMax-- ;
}

void Polytempo_VisualMetro::eventNotification(Polytempo_Event *event)
{
    if(!Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("showVisualMetro")) return;

    if(event->getType() == eventType_Beat)
    {
        if(event->hasProperty(eventPropertyString_Duration))
            beatDuration = event->getProperty(eventPropertyString_Duration);
        else
            beatDuration = 1.0 / timeInterval;
        
        // the subdivision feature is not implemented yet
        beatSubdivision = 0;
        
        if(!event->getProperty(eventPropertyString_Pattern).isVoid())  pattern = event->getProperty(eventPropertyString_Pattern);
        
        if(int(event->getProperty(eventPropertyString_Cue)) == 1)
            foregroundColour = cueColour;
        else
            foregroundColour = normalColour;
      
        shouldStop = false;
        
        /* the look and feel of the conductor's movement is based on three parameters: (1) an exponent that depends on the duration of the beat, (2) a certain point at which the conductor jumps immediately to the max, creating an ictus - see above - and (3) a certain duration through which the max is held.  */
        
        float actualDuration = beatDuration / tempoFactor;
        exponentMain = 0.75 + actualDuration;
        exponentMain = exponentMain > 2.5 ? 2.5 : exponentMain; // clip
        
        /*
         40 / timeInterval looks good on a fast computer.
         60 / timeInterval is needed for slower computers (old mac minis)
         */
        
        holdMax = 60 / timeInterval;
        
        increment = (float)timeInterval / actualDuration * 0.001;

        pos = 0;
        
        startTimer(timeInterval);
    }
    else if(event->getType() == eventType_TempoFactor)
    {
        tempoFactor = event->getProperty("value");
        increment = (float)timeInterval / beatDuration * tempoFactor * 0.001;
    }
    else if(event->getType() == eventType_Pause)
    {
        shouldStop = true;
    }
    else if(event->getType() == eventType_Stop)
    {
        Polytempo_NetworkApplication* const app = dynamic_cast<Polytempo_NetworkApplication*>(JUCEApplication::getInstance());
 
        if(isTimerRunning())
        {
            shouldStop = true;
        }
        else if(app->quitApplication)
        {
            app->applicationShouldQuit();
        }
    }
}

