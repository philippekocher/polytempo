#include "Polytempo_GraphicExportPage.h"
#include "../../Data/Polytempo_Composition.h"


Polytempo_GraphicExportPage::Polytempo_GraphicExportPage(int pageNumber)
{
    // A4 at 300 ppi is 2480 x 3508 pixels
    image.reset(new Image(Image::RGB, 2480, 3508, true));
    
    Graphics g (*image);
    g.setColour(Colours::white);
    g.fillRect(image->getBounds());
    
    g.setColour(Colours::black);
    g.setFont(40.0f);
    g.drawText(String(pageNumber), 0, 3300, 2480, 100, Justification::horizontallyCentred, false);
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

//void Polytempo_GraphicExportPage::resized()
//{}

void Polytempo_GraphicExportPage::drawStaff(int x, int y, int width, int numberOfLines, int linesOffset, String name)
{
    Graphics g (*image);
    
    g.setColour(Colours::black);
    g.setFont(40.0f);
    g.drawText(name, x - 320, y, 200, (numberOfLines - 1) * linesOffset, Justification::verticallyCentred, true);
    
    g.setColour(Colours::grey);
    for(int n=0; n<numberOfLines; n++)
        g.drawHorizontalLine(y + (n * linesOffset), x - 100, x - 20);

    g.setColour(Colours::black);
    for(int n=0; n<numberOfLines; n++)
        g.drawHorizontalLine(y + (n * linesOffset), x, x + width);
}

void Polytempo_GraphicExportPage::drawBarline(int x, int y, int numberOfLines, int linesOffset, String timeSignature)
{
    int y1 = y;
    int y2 = y + numberOfLines * (linesOffset - 1);
    
    if(numberOfLines < 1)
    {
        y1 -= linesOffset;
        y2 += linesOffset;
    }

    Graphics g (*image);
    g.setColour(Colours::black);

    g.drawLine(x, y1, x, y2, 3);
    
    if(timeSignature.isNotEmpty())
    {
        int timeSignatureFontHeight = 80;
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

void Polytempo_GraphicExportPage::drawAuxiliaryLine(int x, int y, int numberOfLines, int linesOffset)
{
    int y1 = y;
    int y2 = y + numberOfLines * (linesOffset - 1);
    
    if(numberOfLines < 1)
    {
        y1 -= linesOffset;
        y2 += linesOffset;
    }

    Graphics g (*image);
    g.setColour(Colours::grey);
    
    g.drawLine(x, y1, x, y2, 1);
}

void Polytempo_GraphicExportPage::drawMarker(String marker, int x, int y)
{
    Graphics g (*image);

    g.setColour(Colours::black);
    g.setFont(30.0f);
    if(markerPos == Point<int>(x,y)) markerPos = Point<int>(x, markerPos.y + 25);
    else                             markerPos = Point<int>(x, y);
    
    g.drawText(marker, markerPos.x - 200, markerPos.y, 400, 40, Justification::horizontallyCentred, true);
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
    
    viewedComponent.reset(new Component());
    setViewedComponent(viewedComponent.get(), false);
    setViewPositionProportionately(0.0, 0.0);
}

Polytempo_GraphicExportViewport::~Polytempo_GraphicExportViewport()
{
    viewedComponent = nullptr;
}

void Polytempo_GraphicExportViewport::paint(Graphics& g)
{}

void Polytempo_GraphicExportViewport::update()
{
    viewedComponent->removeAllChildren();
    pages.clear();
    addPage();

    Polytempo_Composition* composition = Polytempo_Composition::getInstance();
    int pageIndex, sequenceIndex, systemIndex;
    int beatPatternIndex, beatPatternCounter;
    Polytempo_BeatPattern *beatPattern;
    int posX, posY;
    int staffOffset = 0;
    for(sequenceIndex=0;sequenceIndex<composition->getNumberOfSequences();sequenceIndex++)
    {
        pageIndex = 0;
        systemIndex = 0;
        beatPatternIndex = 0;
        beatPatternCounter = 0;
        posX = 0;
        
        int marginLeft = 400;
        int marginRight = 200;
        int marginTop = 0;
        int marginBottom = 200;
        int systemWidth = 2480 - marginLeft - marginRight;
        int systemHeight = 1500;
        int systemsPerPage = 2;
        
        Polytempo_Sequence *sequence = composition->getSequence(sequenceIndex);
        staffOffset += sequence->staffOffset;
        String sequenceName = sequence->showName ? sequence->getName() : String();

        for(Polytempo_Event *event : sequence->getEvents())
        {
            if(!event->hasDefinedTime()) continue;
            
            posX = event->getTime() * 0.2 - (pageIndex * systemsPerPage + systemIndex) * systemWidth;
            posY = marginTop + systemIndex * systemHeight + staffOffset;

            if(posX > systemWidth) // reach the end of a line
            {
                // draw staff
                pages[pageIndex]->drawStaff(marginLeft, posY, systemWidth, sequence->numberOfLines, sequence->lineOffset, sequenceName);

                systemIndex++;

                posX -= systemWidth;
                posY += systemHeight;
                
                if(systemIndex == systemsPerPage)
                {
                    pageIndex++;
                    if(pageIndex == pages.size()) addPage();
                    
                    systemIndex = 0;
                    posY = marginTop + staffOffset;
                }                
            }
            
            posX += marginLeft;

            if(event->getType() == eventType_Beat)
            {
                if(int(event->getProperty(eventPropertyString_Pattern)) < 20)
                {
                    if(beatPatternCounter == 0)
                    {
                        beatPattern = sequence->getBeatPattern(beatPatternIndex++);
                        if(beatPattern != nullptr)
                        {
                            beatPatternCounter = beatPattern->getRepeats();

                            pages[pageIndex]->drawBarline(posX, posY, sequence->numberOfLines, sequence->lineOffset, beatPattern->getPattern());
                        }
                    }
                    else
                    {
                        pages[pageIndex]->drawBarline(posX, posY, sequence->numberOfLines, sequence->lineOffset, String());
                    }
                    beatPatternCounter--;

                }
                else
                    pages[pageIndex]->drawAuxiliaryLine(posX, posY, sequence->numberOfLines-1, sequence->lineOffset);
            }
            else if(event->getType() == eventType_Marker)
            {
                pages[pageIndex]->drawMarker(event->getValue().toString(), posX, posY + (sequence->numberOfLines-1) * sequence->lineOffset);
            }
        }
        
        // draw staff up to the last event
        pages[pageIndex]->drawStaff(marginLeft, marginTop + systemIndex * systemHeight + staffOffset, posX - marginLeft, sequence->numberOfLines, sequence->lineOffset, sequenceName);
    }
}

void Polytempo_GraphicExportViewport::addPage()
{
    int pageHeight = getLocalBounds().getHeight();
    int pageWidth = pageHeight * 0.7070707071;

    Polytempo_GraphicExportPage *page = new Polytempo_GraphicExportPage(pages.size() + 1);
    
    page->setBounds(pages.size() * (pageWidth + 10), 0, pageWidth, pageHeight);
    pages.add(page);
    viewedComponent->addAndMakeVisible(page);
    viewedComponent->setBounds(0, 0, pages.size() * (pageWidth + 10), pageHeight);
}

void Polytempo_GraphicExportViewport::exportImages()
{
    int i = 0;
    for(Polytempo_GraphicExportPage *page : pages)
    {
        page->exportImage(String("~/image")+String(i++)+String(".png"));
    }
}
