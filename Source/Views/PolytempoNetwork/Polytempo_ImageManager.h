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

#ifndef __Polytempo_ImageManager__
#define __Polytempo_ImageManager__

#include "../../Scheduler/Polytempo_EventObserver.h"


class Polytempo_ImageManager : public Polytempo_EventObserver
{
public:
    ~Polytempo_ImageManager();
    
    juce_DeclareSingleton(Polytempo_ImageManager, false);
    
    Image* getImage(var imageID);
    OwnedArray < Image >& getImages();
    OwnedArray < String >& getImageIDs();
    String getFileName(var imageID);
    
    void deleteAll();
    void loadImage(Polytempo_Event*);
    void replaceImage(var imageID, var url);
    
    void eventNotification(Polytempo_Event*);
    
private:
    Array < Polytempo_Event* > loadImageEvents;
    OwnedArray < String > imageIDs;
    OwnedArray < Image > images;
    
};
    
#endif  // __Polytempo_ImageManager__
