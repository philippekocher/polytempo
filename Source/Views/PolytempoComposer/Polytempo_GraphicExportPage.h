#pragma once

#include "../../../POLYTEMPO COMPOSER/JuceLibraryCode/JuceHeader.h"


class Polytempo_GraphicExportPage    : public Component
{
public:
    Polytempo_GraphicExportPage();
    ~Polytempo_GraphicExportPage();

    void paint (Graphics&) override;
    void resized() override;

    void exportImage(String);

private:
    ScopedPointer <Image> image;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_GraphicExportPage)
};

class Polytempo_GraphicExportViewport : public Viewport
{
public:
    Polytempo_GraphicExportViewport();
    ~Polytempo_GraphicExportViewport();
    
    void paint (Graphics&);
    void update();
    
    void exportImages();
    
private:
    ScopedPointer <Component> viewedComponent;
    OwnedArray <Polytempo_GraphicExportPage> pages;
};
