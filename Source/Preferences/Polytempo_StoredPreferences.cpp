#include "Polytempo_StoredPreferences.h"
#include "Polytempo_DefaultPreferences.h"


//==============================================================================
Polytempo_StoredPreferences::Polytempo_StoredPreferences()
{
    defaults.reset((PropertiesFile*) new Polytempo_DefaultPreferences());
    flush();
}

Polytempo_StoredPreferences::~Polytempo_StoredPreferences()
{
    flush();
    props = nullptr;
    defaults = nullptr;
    clearSingletonInstance();
}

juce_ImplementSingleton (Polytempo_StoredPreferences);


PropertiesFile& Polytempo_StoredPreferences::getProps()
{
    return *props;
}

void Polytempo_StoredPreferences::flush()
{
    if (props != nullptr)
    {
        props->setValue("recentFiles", recentFiles.toString());
    }
    else
    {
        PropertiesFile::Options options;
        options.folderName           = "Polytempo/"+JUCEApplication::getInstance()->getApplicationName();
        options.applicationName      = "settings";
        options.filenameSuffix       = "xml";
        options.osxLibrarySubFolder  = "Application Support";

        props.reset(new PropertiesFile(options));
        props->setFallbackPropertySet(defaults.get());
    }

    // recent files
    recentFiles.restoreFromString(props->getValue("recentFiles"));
    recentFiles.removeNonExistentFiles();
}