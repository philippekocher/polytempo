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

#ifndef __Polytempo_ComposerApplication__
#define __Polytempo_ComposerApplication__

#include "../JuceLibraryCode/JuceHeader.h"
#include "Polytempo_ComposerWindow.h"
#include "../../Network/Polytempo_OSCSender.h"


class Polytempo_ComposerApplication : public JUCEApplication
{
public:
    Polytempo_ComposerApplication();
    
    const String getApplicationName() override      { return ProjectInfo::projectName; }
    const String getApplicationVersion() override   { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override      { return false; }
    
    void initialise (const String& commandLine) override;
    void shutdown() override;
    void systemRequestedQuit() override;
    void anotherInstanceStarted(const String& commandLine) override;

    static ApplicationCommandManager& getCommandManager();
//    static DocumentWindow& getDocumentWindow();
    static Polytempo_ComposerMainView& getMainView();
    
private:
    ScopedPointer<Polytempo_ComposerWindow> composerWindow;
    ScopedPointer<ApplicationCommandManager> commandManager;
    ScopedPointer<Polytempo_OSCSender> oscSender;
};


#endif  // __Polytempo_ComposerApplication__
