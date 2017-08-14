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

#ifndef __Polytempo_GraphicsView__
#define __Polytempo_GraphicsView__

//#include "../../JuceLibraryCode/JuceHeader.h"

#include "Polytempo_GraphicsViewRegion.h"
class Polytempo_GraphicsSplitViewRegion;
#include "../../Scheduler/Polytempo_EventObserver.h"


class Polytempo_GraphicsView : public Component,
                               public Polytempo_EventObserver
{
public:
    Polytempo_GraphicsView ();
    ~Polytempo_GraphicsView ();

    void paint(Graphics& g);
    void resized();
    void eventNotification(Polytempo_Event *event);
    
private:
    
    void deleteAll();
    void clearAll();
    
    void addRegion(Polytempo_Event *event);
    void defineSection(Polytempo_Event *event);
//    void addSplitRegion(Polytempo_Event *event);
    void displayImage(Polytempo_Event *event);
    void displayText(Polytempo_Event *event);
    void displayProgessbar(Polytempo_Event *event);

    HashMap < String, Polytempo_GraphicsViewRegion* > regionsMap;
//    HashMap < String, Polytempo_GraphicsSplitViewRegion* > splitRegionsMap;
    HashMap < String, var > sectionBoundsMap;
    HashMap < String, String > sectionImageIDMap;
    
    ScopedPointer < Polytempo_Progressbar > progressbar;
};
    



#endif   // __Polytempo_GraphicsView__
