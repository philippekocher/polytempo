#include "Polytempo_GraphicExportPage.h"

Polytempo_GraphicExportPage::Polytempo_GraphicExportPage()
{
    // A4 at 300 ppi is 2480 x 3508 pixels
    image = new Image(Image::RGB, 2480, 3508, true);
    
    Graphics g (*image);
    g.setColour(Colours::white);
    g.fillRect(image->getBounds());

    g.setColour(Colours::black);
    for(int y=250; y<300; y=y+10)
        g.drawHorizontalLine(y, 200, 2280);

}

Polytempo_GraphicExportPage::~Polytempo_GraphicExportPage()
{
    image = nullptr;
}

void Polytempo_GraphicExportPage::paint (Graphics& g)
{
    Rectangle<int> r = getLocalBounds();
    g.drawImageWithin(*image, r.getX(), r.getY(), r.getWidth(), r.getHeight(), RectanglePlacement::stretchToFit);
}

void Polytempo_GraphicExportPage::resized()
{
}

void Polytempo_GraphicExportPage::exportImage(String filepath)
{
    PNGImageFormat pngWriter;
    FileOutputStream stream ((File (filepath)));
    if (stream.openedOk())
    {
        stream.setPosition (0);
        stream.truncate();
        pngWriter.writeImageToStream(*image, stream);
    }
}


Polytempo_GraphicExportViewport::Polytempo_GraphicExportViewport()
{
    setScrollBarsShown(false, true, false, true);
    
    setViewedComponent(viewedComponent = new Component(), false);
    setViewPositionProportionately(0.0, 0.0);
}

Polytempo_GraphicExportViewport::~Polytempo_GraphicExportViewport()
{
    viewedComponent = nullptr;
}

void Polytempo_GraphicExportViewport::paint(Graphics& g)
{
    //g.fillAll(Colours::red);
}

void Polytempo_GraphicExportViewport::update()
{
    int pageHeight = getLocalBounds().getHeight();
    int pageWidth = pageHeight * 0.7070707071;
    
    viewedComponent->setBounds(0,0,2000,pageHeight);

    viewedComponent->removeAllChildren();
    pages.clear();

    Polytempo_GraphicExportPage *page;
    for(int i=0; i<1; i++)
    {
        pages.add(page = new Polytempo_GraphicExportPage());
        viewedComponent->addAndMakeVisible(page);
        page->setBounds(i * (pageWidth + 10),0,pageWidth,pageHeight);
    }
}

void Polytempo_GraphicExportViewport::exportImages()
{
    int i = 0;
    for(Polytempo_GraphicExportPage *page : pages)
    {
        page->exportImage(String("~/image")+String(i++)+String(".png"));
    }
}
