#include "Polytempo_GraphicExportView.h"
#include "../../Preferences/Polytempo_StoredPreferences.h"


#define SETTINGS_HEIGHT 40
#define SEQUENCES_HEIGHT 120

// A4 at 300 ppi is 2480 x 3508 pixels
// this is 150 ppi:
#define PAGE_IMAGE_WIDTH 1240
#define PAGE_IMAGE_HEIGHT 1754

Polytempo_GraphicExportView::Polytempo_GraphicExportView()
{
    addAndMakeVisible(graphicExportViewport);
    addAndMakeVisible(graphicExportSettingsComponent);
    addAndMakeVisible(sequencesViewport);
}

Polytempo_GraphicExportView::~Polytempo_GraphicExportView()
{}

void Polytempo_GraphicExportView::paint(Graphics& /*g*/)
{}

void Polytempo_GraphicExportView::resized()
{
    Rectangle<int> r (getLocalBounds());

    graphicExportViewport.setBounds(r.withTrimmedBottom(SEQUENCES_HEIGHT + SETTINGS_HEIGHT));
    graphicExportSettingsComponent.setBounds(r.withY(r.getHeight() - SEQUENCES_HEIGHT - SETTINGS_HEIGHT).withHeight(SETTINGS_HEIGHT));
    sequencesViewport.setBounds(r.removeFromBottom(SEQUENCES_HEIGHT));
    sequencesViewport.showGraphicalSettings(true);

    update();
}

void Polytempo_GraphicExportView::setTimeFactor(float factor)
{
    timeFactor = factor;
    update();
}

int Polytempo_GraphicExportView::getSystemHeight()
{
    return systemHeight;
}

void Polytempo_GraphicExportView::setSystemHeight(int height)
{
    systemHeight = height;
    update();
}

void Polytempo_GraphicExportView::setLandscape(bool format)
{
    landscape = format;
    update();
}


void Polytempo_GraphicExportView::update()
{
    graphicExportViewport.viewedComponent->removeAllChildren();
    pages.clear();
    addPage();
    
    Polytempo_Composition* composition = Polytempo_Composition::getInstance();
    if(numberOfSequences != composition->getNumberOfSequences())
    {
        numberOfSequences = composition->getNumberOfSequences();
        if(numberOfSequences < 1) return;
        systemHeight = numberOfSequences * 130;
        graphicExportSettingsComponent.update(this);
    }
 
    int staveOffset = 0;
    int marginLeft = 200;
    int marginRight = 100;
    int marginTop = 0;
    int systemWidth = landscape ?
        PAGE_IMAGE_HEIGHT - marginLeft - marginRight :
        PAGE_IMAGE_WIDTH - marginLeft - marginRight;
    int systemsPerPage = landscape ?
        int(PAGE_IMAGE_WIDTH / systemHeight) :
        int(PAGE_IMAGE_HEIGHT / systemHeight);
    int systemCount = 1;

    for(int sequenceIndex = 0; sequenceIndex < numberOfSequences; sequenceIndex++)
    {
        int pageIndex = 0;
        int systemIndex = 0;
        int beatPatternIndex = 0;
        int beatPatternCounter = 0;
        int posX = 0;
        int posY;

        Polytempo_Sequence *sequence = composition->getSequence(sequenceIndex);
        staveOffset += sequence->staveOffset;
        Polytempo_BeatPattern *beatPattern;

        for(Polytempo_Event *event : sequence->getEvents())
        {
            if(!event->hasDefinedTime()) continue;
            
            posX = int(event->getTime() * timeFactor * 0.1 - (pageIndex * systemsPerPage + systemIndex%systemsPerPage) * systemWidth);
            posY = marginTop + systemIndex % systemsPerPage * systemHeight + staveOffset;
            
            while(posX > systemWidth) // reach the end of a line
            {
                // draw staves
                pages[pageIndex]->drawStaves(marginLeft, posY, systemWidth, sequence->numberOfStaves, sequence->secondaryStaveOffset, sequence->numberOfLines, sequence->lineOffset);
                
                systemIndex++;
                
                posX -= systemWidth;
                posY += systemHeight;
                
                if(systemIndex % systemsPerPage == 0 )
                {
                    pageIndex++;
                    if(pageIndex == pages.size()) addPage();
                    
                    posY = marginTop + staveOffset;
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
                            
                            pages[pageIndex]->drawBarline(posX, posY, sequence->numberOfStaves, sequence->secondaryStaveOffset, sequence->numberOfLines, sequence->lineOffset, beatPattern->getPattern());
                        }
                        else
                        {
                            // end of beat pattern: draw a double barline
                            pages[pageIndex]->drawBarline(posX - 5, posY, sequence->numberOfStaves, sequence->secondaryStaveOffset, sequence->numberOfLines, sequence->lineOffset, String());
                            pages[pageIndex]->drawBarline(posX + 1, posY, sequence->numberOfStaves, sequence->secondaryStaveOffset, sequence->numberOfLines, sequence->lineOffset, String());
                        }
                    }
                    else
                    {
                        pages[pageIndex]->drawBarline(posX, posY, sequence->numberOfStaves, sequence->secondaryStaveOffset, sequence->numberOfLines, sequence->lineOffset, String());
                    }
                    beatPatternCounter--;
                    
                }
                else
                    pages[pageIndex]->drawAuxiliaryLine(posX, posY, sequence->numberOfStaves, sequence->secondaryStaveOffset, sequence->numberOfLines, sequence->lineOffset);
            }
            else if(event->getType() == eventType_Marker)
            {
                pages[pageIndex]->drawMarker(event->getValue().toString(), posX, posY, sequence->numberOfStaves, sequence->secondaryStaveOffset, sequence->numberOfLines, sequence->lineOffset);
            }
        }
        
        // draw staves up to the last event
        pages[pageIndex]->drawStaves(marginLeft, marginTop + systemIndex%systemsPerPage * systemHeight + staveOffset, posX - marginLeft, sequence->numberOfStaves, sequence->secondaryStaveOffset, sequence->numberOfLines, sequence->lineOffset);
        systemIndex++;
        
        if(systemIndex > systemCount) systemCount = systemIndex;
    }
    
    staveOffset = 0;
    for(int sequenceIndex = 0; sequenceIndex < numberOfSequences; sequenceIndex++)
    {
        Polytempo_Sequence *sequence = composition->getSequence(sequenceIndex);
        String sequenceName = sequence->showName ? sequence->getName() : String();
        int pageIndex = -1;
        staveOffset += sequence->staveOffset;
        for(int systemIndex = 0; systemIndex < systemCount; systemIndex++)
        {
            int posY = marginTop + systemIndex%systemsPerPage * systemHeight + staveOffset;

            if(systemIndex%systemsPerPage == 0)
                pageIndex++;

            pages[pageIndex]->drawStaveBeginning(marginLeft, posY, sequence->numberOfStaves, sequence->secondaryStaveOffset, sequence->numberOfLines, sequence->lineOffset, sequenceName);

        }
    }
}

void Polytempo_GraphicExportView::addPage()
{
    int pageHeight = graphicExportViewport.getLocalBounds().getHeight();
    int pageWidth = landscape ? int(pageHeight * 1.414516129032258) : int(pageHeight * 0.706955530216648);
    
    Polytempo_GraphicExportPage *page = landscape ?
    new Polytempo_GraphicExportPage(pages.size() + 1, PAGE_IMAGE_HEIGHT, PAGE_IMAGE_WIDTH) :
    new Polytempo_GraphicExportPage(pages.size() + 1, PAGE_IMAGE_WIDTH, PAGE_IMAGE_HEIGHT);

    page->setBounds(pages.size() * (pageWidth + 10), 0, pageWidth, pageHeight);
    pages.add(page);
    graphicExportViewport.viewedComponent->addAndMakeVisible(page);
    graphicExportViewport.viewedComponent->setBounds(0, 0, pages.size() * (pageWidth + 10), pageHeight);
}

void Polytempo_GraphicExportView::exportPages()
{
    File directory(Polytempo_StoredPreferences::getInstance()->getProps().getValue("compositionFileDirectory")+"/image#.jpg");
    FileChooser fileChooser("Export Pages", directory, "*.jpg", true);
    
    if(fileChooser.browseForFileToSave(true))
    {
        int i = 0;
        for(Polytempo_GraphicExportPage *page : pages)
        {
            page->exportImage(fileChooser.getResult().getFullPathName().replace("#",String(++i).paddedLeft('0',3)));
        }
    }
}
