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

#ifndef __Polytempo_Alerts__
#define __Polytempo_Alerts__


class Polytempo_Alert
{
public:
    static void show(const String& title,
                     const String& message,
                     Component* associatedComponent = nullptr)
    {
        AlertWindow::showMessageBox(AlertWindow::WarningIcon, title, message, "OK", associatedComponent);        
    };
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_Alert)
};

class Polytempo_OkCancelAlert
{
public:
    static bool show(const String& title,
                     const String& message)
    {
        return AlertWindow::showOkCancelBox (AlertWindow::QuestionIcon,
                                             title, message);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_OkCancelAlert)
};

class Polytempo_YesNoCancelAlert
{
public:
    static void show(const String& title,
                     const String& message,
                     ModalComponentManager::Callback *callback)
    {
        AlertWindow::showYesNoCancelBox(AlertWindow::InfoIcon,
                                        title, message,
                                        String::empty,
                                        String::empty,
                                        String::empty,
                                        nullptr,
                                        callback);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_YesNoCancelAlert)
};


#endif  // __Polytempo_Alerts__
