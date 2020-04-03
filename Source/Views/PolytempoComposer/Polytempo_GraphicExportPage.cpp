#include "Polytempo_GraphicExportPage.h"
#include "../../Data/Polytempo_Composition.h"


Polytempo_GraphicExportPage::Polytempo_GraphicExportPage(int pageNumber, int pageWidth, int pageHeight)
{
    image.reset(new Image(Image::RGB, pageWidth, pageHeight, true));
    
    Graphics g (*image);
    g.setColour(Colours::white);
    g.fillRect(image->getBounds());
    
    g.setColour(Colours::black);
    g.setFont(20.0f);
    g.drawText(String(pageNumber), 0, pageHeight * 0.9, pageWidth, pageHeight * 0.1, Justification::horizontallyCentred, false);
}

Polytempo_GraphicExportPage::~Polytempo_GraphicExportPage()
{
    image = nullptr;
}

void Polytempo_GraphicExportPage::paint(Graphics& g)
{
    Rectangle<int> r = getLocalBounds();
    g.drawImageWithin(*image, r.getX(), r.getY(), r.getWidth(), r.getHeight(), RectanglePlacement::stretchToFit);
}

void Polytempo_GraphicExportPage::drawStaves(int x, int y, int width, int numberOfStaves, int secondaryStaveOffset, int numberOfLines, int linesOffset)
{
    Graphics g (*image);
    
    g.setColour(Colours::black);
    for(int i = 0; i < numberOfStaves; i++)
    {
        for(int n = 0; n < numberOfLines; n++)
            g.drawHorizontalLine(y + i * secondaryStaveOffset + n * linesOffset, x, x + width);
    }
}

void Polytempo_GraphicExportPage::drawStaveBeginning(int x, int y, int numberOfStaves, int secondaryStaveOffset, int numberOfLines, int linesOffset, String name)
{

    Graphics g (*image);

    g.setColour(Colours::grey);
    for(int i = 0; i < numberOfStaves; i++)
    {
        for(int n=0; n<numberOfLines; n++)
            g.drawHorizontalLine(y + i * secondaryStaveOffset + n * linesOffset, x - 40, x - 10);
    }

    if(numberOfLines < 2)
    {
        y -= linesOffset * 0.5;
        numberOfLines = 2;
    }

    g.setColour(Colours::black);
    g.setFont(20.0f);
    g.drawText(name, x - 140, y, 100, (numberOfStaves - 1) * secondaryStaveOffset + (numberOfLines - 1) * linesOffset, Justification::verticallyCentred, true);
    
}

void Polytempo_GraphicExportPage::drawBarline(int x, int y, int numberOfStaves, int secondaryStaveOffset, int numberOfLines, int linesOffset, String timeSignature)
{
    int y1 = y;
    int y2 = y + (numberOfStaves - 1) * secondaryStaveOffset + (numberOfLines - 1) * linesOffset + 1;
    
    if(numberOfLines < 2)
    {
        y1 -= linesOffset;
        y2 += linesOffset;
    }

    Graphics g (*image);
    g.setColour(Colours::black);

    g.drawLine(x, y1, x, y2, 3);
    
    if(timeSignature.isNotEmpty())
    {
        int timeSignatureFontHeight = 24;
        StringArray tokens;
        tokens.addTokens(timeSignature, "/", "");
        tokens.removeEmptyStrings();
        tokens.trim();
        
        g.setFont(timeSignatureFontHeight);
        g.drawText(tokens[0],
                   x - 200,
                   y1 - timeSignatureFontHeight * 1.75,
                   400,
                   timeSignatureFontHeight,
                   Justification::horizontallyCentred, false);
        g.drawText(tokens[1],
                   x - 200,
                   y1 - timeSignatureFontHeight,
                   400,
                   timeSignatureFontHeight,
                   Justification::horizontallyCentred, false);
    }
}

void Polytempo_GraphicExportPage::drawAuxiliaryLine(int x, int y, int numberOfStaves, int secondaryStaveOffset, int numberOfLines, int linesOffset)
{
    int y1 = y;
    int y2 = y + (numberOfStaves - 1) * secondaryStaveOffset + (numberOfLines - 1) * linesOffset;

    if(numberOfLines < 2)
    {
        y1 -= linesOffset;
        y2 += linesOffset;
    }

    Graphics g (*image);
    g.setColour(Colours::grey);
    
    g.drawLine(x, y1, x, y2, 1);
}

void Polytempo_GraphicExportPage::drawMarker(String marker, int x, int y, int numberOfStaves, int secondaryStaveOffset, int numberOfLines, int linesOffset)
{
    if(numberOfLines < 2)
        numberOfLines = 2;
    y = y + (numberOfStaves - 1) * secondaryStaveOffset + (numberOfLines - 1) * linesOffset;

    Graphics g (*image);

    g.setColour(Colours::black);
    g.setFont(16.0f);
    if(markerPos == Point<int>(x,y)) markerPos = Point<int>(x, markerPos.y + 16);
    else                             markerPos = Point<int>(x, y);
    
    g.drawText(marker, markerPos.x - 200, markerPos.y, 400, 20, Justification::horizontallyCentred, true);
}


void Polytempo_GraphicExportPage::exportImage(String filepath)
{
    PNGImageFormat pngWriter;
    JPEGImageFormat jpgWriter;
    FileOutputStream stream ((File (filepath)));
    if (stream.openedOk())
    {
        stream.setPosition (0);
        stream.truncate();
        //pngWriter.writeImageToStream(*image, stream);
        jpgWriter.writeImageToStream(*image, stream);
    }
}


Polytempo_GraphicExportViewport::Polytempo_GraphicExportViewport()
{
    setScrollBarsShown(false, true, false, true);
    
    viewedComponent.reset(new Component());
    setViewedComponent(viewedComponent.get(), false);
    setViewPositionProportionately(0.0, 0.0);
}

Polytempo_GraphicExportViewport::~Polytempo_GraphicExportViewport()
{}
