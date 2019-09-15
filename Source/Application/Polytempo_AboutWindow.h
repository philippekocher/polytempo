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

#ifndef __Polytempo_AboutWindow__
#define __Polytempo_AboutWindow__

#include "../JuceLibraryCode/JuceHeader.h"

class Polytempo_AboutWindow: public Component
{
public:
    Polytempo_AboutWindow();
    ~Polytempo_AboutWindow();

    static void show();
    void closeButtonPressed();
    
private:
    std::unique_ptr<Label> label1;
	std::unique_ptr<Label> label2;
	std::unique_ptr<Label> label3;
	std::unique_ptr<Label> label4;
	std::unique_ptr<ImageComponent> iconComponent;
	std::unique_ptr<HyperlinkButton> link1;
	std::unique_ptr<HyperlinkButton> link2;
	std::unique_ptr<Component> aboutComponent;
};


#endif  // __Polytempo_AboutWindow__
