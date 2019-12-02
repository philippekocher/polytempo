#ifndef __Polytempo_StoredPreferences__
#define __Polytempo_StoredPreferences__

#include "JuceHeader.h"


//==============================================================================
/**
    A singleton to hold the jucer's persistent settings, and to save them in a
    suitable PropertiesFile.
*/
class Polytempo_StoredPreferences
{
public:
    Polytempo_StoredPreferences();
    ~Polytempo_StoredPreferences();

    juce_DeclareSingleton (Polytempo_StoredPreferences, false);

    PropertiesFile& getProps();
    void flush();

    RecentlyOpenedFilesList recentFiles;

    //Array <Colour> swatchColours;

private:
	std::unique_ptr<PropertiesFile> props;
	std::unique_ptr<PropertiesFile> defaults;
};


#endif   // __Polytempo_StoredPreferences__
