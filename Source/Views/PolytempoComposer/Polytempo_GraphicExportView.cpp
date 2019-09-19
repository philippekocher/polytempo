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
    
    graphicExportSettingsComponent.setGraphicExportView(this);
}

Polytempo_GraphicExportView::~Polytempo_GraphicExportView()
{}

void Polytempo_GraphicExportView::paint(Graphics& g)
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
        
        int marginLeft = 200;
        int marginRight = 100;
        int marginTop = 0;
        int marginBottom = 200;
        int systemWidth = landscape ?
            PAGE_IMAGE_HEIGHT - marginLeft - marginRight :
            PAGE_IMAGE_WIDTH - marginLeft - marginRight;
        int systemsPerPage = 2;
        
        Polytempo_Sequence *sequence = composition->getSequence(sequenceIndex);
        staffOffset += sequence->staffOffset;
        String sequenceName = sequence->showName ? sequence->getName() : String();
        
        for(Polytempo_Event *event : sequence->getEvents())
        {
            if(!event->hasDefinedTime()) continue;
            
            posX = event->getTime() * timeFactor * 0.1 - (pageIndex * systemsPerPage + systemIndex) * systemWidth;
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
                    pages[pageIndex]->drawAuxiliaryLine(posX, posY, sequence->numberOfLines, sequence->lineOffset);
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

void Polytempo_GraphicExportView::addPage()
{
    int pageHeight = graphicExportViewport.getLocalBounds().getHeight();
    int pageWidth = landscape ? pageHeight * 1.414516129032258 : pageHeight * 0.706955530216648;
    
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
