#include "Polytempo_BeatPatternListComponent.h"
#include "../../Data/Polytempo_Composition.h"
#include "../../Application/PolytempoComposer/Polytempo_ComposerApplication.h"
#include "../../Application/Polytempo_CommandIDs.h"


Polytempo_BeatPatternListComponent::Polytempo_BeatPatternListComponent()
{
    addAndMakeVisible(table = new Polytempo_TableListBox());
    table->setModel(this);
                                       // id, width, min, max, flags
    table->getHeader().addColumn("Pattern", 1, 10, 70, -1, TableHeaderComponent::visible);
    table->getHeader().addColumn("Repeats", 2, 10, -1, -1, TableHeaderComponent::visible);
    table->getHeader().addColumn("Counter", 3, 10, -1, -1, TableHeaderComponent::visible);
    table->getHeader().addColumn("Marker",  4, 10, -1, -1, TableHeaderComponent::visible);

    table->getHeader().setPopupMenuActive(false);
    table->getHeader().setStretchToFitActive(true);
    
    table->setColour(ListBox::backgroundColourId, Colour(245,245,245));
}

Polytempo_BeatPatternListComponent::~Polytempo_BeatPatternListComponent()
{
    deleteAllChildren();
}

void Polytempo_BeatPatternListComponent::paint(Graphics& g)
{
    if(sequence == nullptr || sequence != Polytempo_Composition::getInstance()->getSelectedSequence())
        setSequence();
  
    if(sequence == nullptr) return;
    
    g.fillAll(sequence->getColour());

    if(getNumCurrentlyModalComponents() == 0)
        table->updateContent();
}


String Polytempo_BeatPatternListComponent::getText(int rowNumber, int columnId)
{
    Polytempo_BeatPattern* beatPattern = Polytempo_Composition::getInstance()->getSelectedSequence()->getBeatPattern(rowNumber);
    String text;
    
    switch(columnId)
    {
        case 1:
            text = String(beatPattern->getPattern());
            break;
        case 2:
            text = String(beatPattern->getRepeats());
            break;
        case 3:
            text = String(beatPattern->getCounter());
            break;
        case 4:
            text = String(beatPattern->getMarker());
            break;
        default:
            text = "--";
    }
    
    return text;
}

void Polytempo_BeatPatternListComponent::setText(String text, int rowNumber, int columnId)
{
    Polytempo_BeatPattern* beatPattern = Polytempo_Composition::getInstance()->getSelectedSequence()->getBeatPattern(rowNumber);
    
    EditableTextCustomComponent* cell = (EditableTextCustomComponent*)table->getCellComponent(columnId, rowNumber);
    // overwrite the value immediately with the actual value
    
    
    switch(columnId)
    {
        case 1:
            beatPattern->setPattern(text);
            break;
        case 2:
            beatPattern->setRepeats(text.getIntValue());
            cell->setText(String(beatPattern->getRepeats()), dontSendNotification);
            break;
        case 3:
            beatPattern->setCounter(text);
            cell->setText(beatPattern->getCounter(), dontSendNotification);
            break;
        case 4:
            beatPattern->setMarker(text);
            break;
    }
    
    
    Polytempo_Composition::getInstance()->getSelectedSequence()->buildBeatPattern();
}

int Polytempo_BeatPatternListComponent::getNumRows()
{
    if(!Polytempo_Composition::getInstance()->getSelectedSequence()) return 0;
    return Polytempo_Composition::getInstance()->getSelectedSequence()->getBeatPatterns()->size();
}

void Polytempo_BeatPatternListComponent::selectedRowsChanged(int index)
{
    Polytempo_Composition::getInstance()->getSelectedSequence()->setSelectedBeatPatternIndex(index);
    Polytempo_ComposerApplication::getMainView().repaint();
}

void Polytempo_BeatPatternListComponent::paintCell(Graphics& /*g*/, int /*rowNumber*/, int /*columnId*/, int /*width*/, int /*height*/, bool /*rowIsSelected*/)
{}

Component* Polytempo_BeatPatternListComponent::refreshComponentForCell(int rowNumber, int columnId, bool rowIsSelected, Component* existingComponentToUpdate)
{
    EditableTextCustomComponent* textLabel = static_cast<EditableTextCustomComponent*> (existingComponentToUpdate);
    
    // If an existing component is being passed-in for updating, we'll re-use it,
    // but if not, we'll have to create one.
    if (textLabel == nullptr)
        textLabel = new EditableTextCustomComponent(this);
    
    textLabel->setText(getText(rowNumber, columnId), dontSendNotification);
    textLabel->setRowAndColumn(rowNumber, columnId);
    textLabel->setFont(Font (13.0f, rowIsSelected ? Font::bold : Font::plain));
    textLabel->setColour(Label::textColourId, rowIsSelected ? Colour(0,0,0) : Colour(90,90,90));
    
    return textLabel;
}

void Polytempo_BeatPatternListComponent::showPopupMenu()
{
    ApplicationCommandManager* commandManager = &Polytempo_ComposerApplication::getCommandManager();
    
    PopupMenu m;
    
    m.addCommandItem(commandManager, Polytempo_CommandIDs::addBeatPattern);
    m.addCommandItem(commandManager, Polytempo_CommandIDs::insertBeatPattern);
    m.addCommandItem(commandManager, Polytempo_CommandIDs::removeBeatPattern);

    m.show();
}

void Polytempo_BeatPatternListComponent::setSequence()
{
    sequence = Polytempo_Composition::getInstance()->getSelectedSequence();
    if(sequence == nullptr) return;
    
    sequence->setBeatPatternListComponent(this);
    
    table->deselectAllRows();
    sequence->setSelectedBeatPatternIndex(-1);
    
    if(getNumRows() == 0) focusRow = 0; // ensure focus when a first beat pattern is added
    else                  focusRow = -1;
}
