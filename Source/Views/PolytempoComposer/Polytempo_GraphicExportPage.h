#pragma once

#include "JuceHeader.h"

class Polytempo_GraphicExportPage : public Component
{
public:
    Polytempo_GraphicExportPage(int pageNumber, int, int);
    ~Polytempo_GraphicExportPage() override;

    void paint (Graphics&) override;
    
    void drawStaves(int x, int y, int width, int numberOfStaves, int secondaryStaveOffset, int numberOfLines, int linesOffset);
    void drawStaveBeginning(int x, int y, int numberOfStaves, int secondaryStaveOffset, int numberOfLines, int linesOffset, String name);
    void drawBarline(int x, int y, int numberOfStaves, int secondaryStaveOffset, int numberOfLines, int linesOffset, String timeSignature);
    void drawAuxiliaryLine(int x, int y, int numberOfStaves, int secondaryStaveOffset, int numberOfLines, int linesOffset);
    void drawMarker(String marker, int x, int y, int numberOfStaves, int secondaryStaveOffset, int numberOfLines, int linesOffset);

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
    std::unique_ptr<Component> viewedComponent;
};
