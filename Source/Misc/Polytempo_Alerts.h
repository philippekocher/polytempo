#pragma once

class Polytempo_Alert
{
public:
    static void show(const String& title,
                     const String& message,
                     Component* associatedComponent = nullptr)
    {
        AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, title, message, "OK", associatedComponent);
    };
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_Alert)
};

class Polytempo_OkCancelAlert
{
public:
    static void show(const String& title,
                     const String& message,
                     ModalComponentManager::Callback *callback)
    {
        AlertWindow::showOkCancelBox (AlertWindow::QuestionIcon, title, message,
        {}, {}, nullptr, callback);
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
                                        String(),
                                        String(),
                                        String(),
                                        nullptr,
                                        callback);
    }

    enum callbackTag
    {
        applicationQuitTag = 0,
        openDocumentTag,
        newDocumentTag,
    };

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_YesNoCancelAlert)
};