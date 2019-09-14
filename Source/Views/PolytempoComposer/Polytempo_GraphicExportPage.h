#pragma once

#include "../JuceLibraryCode/JuceHeader.h"


class Polytempo_GraphicExportPage : public Component
{
public:
    Polytempo_GraphicExportPage(int pageNumber);
    ~Polytempo_GraphicExportPage();

    void paint (Graphics&) override;
//    void resized() override;
    
    void drawStaff (int x1, int x2, int y, int numberOfLines, int linesOffset, String name);
    void drawBarline(int x, int y1, int y2, String timeSignature);
    void drawAuxiliaryLine(int x, int y1, int y2);
    void drawMarker(String marker, int x, int y);

    void exportImage(String);

private:
    std::unique_ptr<Image> image;
    Point<int> markerPos;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_GraphicExportPage)
};

class Polytempo_GraphicExportViewport : public Viewport
{
public:
    Polytempo_GraphicExportViewport();
    ~Polytempo_GraphicExportViewport();
    
    void paint (Graphics&);
    void update();
    void addPage();
    
    void exportImages();
    
private:
    std::unique_ptr<Component> viewedComponent;
    OwnedArray <Polytempo_GraphicExportPage> pages;
};
