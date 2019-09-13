#include "Polytempo_GraphicExportView.h"


Polytempo_GraphicExportView::Polytempo_GraphicExportView()
{
    addAndMakeVisible(graphicExportViewport);
    addAndMakeVisible(sequencesViewport);
}

Polytempo_GraphicExportView::~Polytempo_GraphicExportView()
{
}

void Polytempo_GraphicExportView::paint(Graphics& g)
{
}

void Polytempo_GraphicExportView::resized()
{
    Rectangle<int> r (getLocalBounds());

    graphicExportViewport.setBounds(r.withTrimmedBottom(120));
    sequencesViewport.setBounds(r.removeFromBottom(120));
    
    graphicExportViewport.update();
    graphicExportViewport.exportImages();
}
