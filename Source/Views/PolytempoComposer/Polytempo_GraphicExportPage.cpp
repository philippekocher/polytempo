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
    g.drawText(String(pageNumber), 0, int(pageHeight * 0.9), pageWidth, int(pageHeight * 0.1), Justification::horizontallyCentred, false);
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
            g.drawHorizontalLine(y + i * secondaryStaveOffset + n * linesOffset, float(x), float(x + width));
    }
}

void Polytempo_GraphicExportPage::drawStaveBeginning(int x, int y, int numberOfStaves, int secondaryStaveOffset, int numberOfLines, int linesOffset, String name)
{

    Graphics g (*image);

    g.setColour(Colours::grey);
    for(int i = 0; i < numberOfStaves; i++)
    {
        for(int n=0; n<numberOfLines; n++)
            g.drawHorizontalLine(y + i * secondaryStaveOffset + n * linesOffset, float(x - 40), float(x - 10));
    }

    if(numberOfLines < 2)
    {
        y -= int(linesOffset * 0.5);
        numberOfLines = 2;
    }

    g.setColour(Colours::black);
    g.setFont(20.0f);
    g.drawText(name, x - 140, y, 100, (numberOfStaves - 1) * secondaryStaveOffset + (numberOfLines - 1) * linesOffset, Justification::verticallyCentred, true);
    
}

void Polytempo_GraphicExportPage::drawBarline(int x, int y, int numberOfStaves, int secondaryStaveOffset, int numberOfLines, int linesOffset, StringArray printableTimeSignature)
{
    int x1 = x;
    int y1 = y;
    int y2 = y + (numberOfStaves - 1) * secondaryStaveOffset + (numberOfLines - 1) * linesOffset + 1;
    
    if(numberOfLines < 2)
    {
        y1 -= linesOffset;
        y2 += linesOffset;
    }

    Graphics g (*image);
    g.setColour(Colours::black);

    g.drawLine(float(x), float(y1), float(x), float(y2), 3.0f);
    
    if(!printableTimeSignature.isEmpty())
    {
        GlyphArrangement glyphs = GlyphArrangement();
        GlyphArrangement tempGlyphs = GlyphArrangement();
        float timeSignatureFontHeight = 24.0f;
        Font font = Font(timeSignatureFontHeight);
        float width, height;
                
        for(int i=0; i<int(printableTimeSignature.size()*0.5); i++)
        {
            if(i>0)
            {
                tempGlyphs.clear();
                tempGlyphs.addLineOfText(font, "+", (float)x1, (float)y1 - timeSignatureFontHeight * 0.5f);
                glyphs.addGlyphArrangement(tempGlyphs);
                width = tempGlyphs.getBoundingBox(0,-1,true).getWidth();
                x1 += int(width+1);
            }
            
            String num = printableTimeSignature[i*2];
            String den = printableTimeSignature[i*2+1];

            tempGlyphs.clear();
            tempGlyphs.addLineOfText(font, num, (float)x1, 0.0f);
            tempGlyphs.addLineOfText(font, den, (float)x1, 0.0f);
            
            width = tempGlyphs.getBoundingBox(0,-1,true).getWidth();
            tempGlyphs.justifyGlyphs(0, num.length(), (float)x1, (float)y1 - timeSignatureFontHeight * 1.25f, width, 0, Justification::horizontallyCentred);
            tempGlyphs.justifyGlyphs(num.length(), den.length(), (float)x1, (float)y1 - timeSignatureFontHeight * 0.5f, width, 0, Justification::horizontallyCentred);

            glyphs.addGlyphArrangement(tempGlyphs);
            x1 += int(width+1);
        }
        width = glyphs.getBoundingBox(0,-1,true).getWidth();
        height = glyphs.getBoundingBox(0,-1,true).getHeight();
        glyphs.justifyGlyphs(0, glyphs.getNumGlyphs(), x - width * 0.5f, y1 - timeSignatureFontHeight * 1.75f, width, height, Justification::horizontallyCentred);

        glyphs.draw(g);
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
    
    g.drawLine(float(x), float(y1), float(x), float(y2), 1.0f);
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
