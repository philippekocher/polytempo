#include "JuceHeader.h"
#include "../../Application/PolytempoComposer/Polytempo_ComposerApplication.h"
#include "Polytempo_PointListComponent.h"
#include "../../Data/Polytempo_Composition.h"
#include "../../Application/Polytempo_CommandIDs.h"
#include "../../Misc/Polytempo_TempoMeasurement.h"


Polytempo_PointListComponent::Polytempo_PointListComponent()
{
    addAndMakeVisible(table = new Polytempo_TableListBox());
    table->setModel(this);
    table->setMultipleSelectionEnabled(true);
    
                                        // id, width, min, max, flags
    table->getHeader().addColumn("Time",       1, 50, 50, -1, TableHeaderComponent::visible);
    table->getHeader().addColumn("Position",   2, 50, 50, -1, TableHeaderComponent::visible);
    table->getHeader().addColumn("Tempo In",   3, 50, 50, -1, TableHeaderComponent::visible);
    table->getHeader().addColumn("Tempo Out",  4, 50, 50, -1, TableHeaderComponent::visible);
    table->getHeader().addColumn("Start",      5, 50, 40, 40, TableHeaderComponent::visible);
    table->getHeader().addColumn("Cue In",     6, 50, 50, -1, TableHeaderComponent::visible);
    
    table->getHeader().setPopupMenuActive(false);
    table->getHeader().setStretchToFitActive(true);
    
    table->setColour(ListBox::backgroundColourId, Colour(245,245,245));
}

Polytempo_PointListComponent::~Polytempo_PointListComponent()
{
    deleteAllChildren();
}

void Polytempo_PointListComponent::paint(Graphics& g)
{
    Polytempo_Sequence* sequence = Polytempo_Composition::getInstance()->getSelectedSequence();
    if(sequence == nullptr) return;
    
    g.fillAll(sequence->getColour());
    
    table->updateContent();

    Polytempo_Composition* composition = Polytempo_Composition::getInstance();
    SparseSet<int>* indices = new SparseSet<int>();
    for(int index : *composition->getSelectedControlPointIndices())
    {
        indices->addRange(Range<int>(index,index+1));
    }
    table->setSelectedRows(*indices, dontSendNotification);
}


String Polytempo_PointListComponent::getText(int rowNumber, int columnId)
{
    Polytempo_Sequence* sequence = Polytempo_Composition::getInstance()->getSelectedSequence();
    Polytempo_ControlPoint* controlPoint = sequence->getControlPoint(rowNumber);
    String text;
    
    switch(columnId)
    {
        case 1:
            text = String(controlPoint->time);
            break;
        case 2:
            text = controlPoint->positionString;
            break;
        case 3:
            text = String(Polytempo_TempoMeasurement::decodeTempoForUI(controlPoint->tempoIn));
            break;
        case 4:
            text = String(Polytempo_TempoMeasurement::decodeTempoForUI(controlPoint->tempoOut));
            break;
        case 5:
            text = String((int)controlPoint->start);
            break;
        case 6:
            text = controlPoint->cueIn.getPattern();
            break;
        default:
            text = "--";
    }
    
    return text;
}

void Polytempo_PointListComponent::setText(String text, int rowNumber, int columnId)
{
    Polytempo_Sequence* sequence = Polytempo_Composition::getInstance()->getSelectedSequence();

    switch(columnId)
    {
        case 1:
            sequence->setControlPointTime(rowNumber, text.getFloatValue());
            sequence->update();
            break;
        case 2:
            sequence->setControlPointPosition(rowNumber, text);
            sequence->update();
            break;
        case 3:
            sequence->setControlPointTempos(rowNumber, Polytempo_TempoMeasurement::encodeTempoFromUI(text.getFloatValue()), -1);
            break;
        case 4:
            sequence->setControlPointTempos(rowNumber, -1, Polytempo_TempoMeasurement::encodeTempoFromUI(text.getFloatValue()));
            break;
        case 5:
            sequence->setControlPointStart(rowNumber, text.getIntValue());
            sequence->update();
            break;
        case 6:
            sequence->setControlPointCue(rowNumber, text);
            sequence->update();
            break;
    }

    Polytempo_Composition::getInstance()->updateContent(); // repaint everything
}

int Polytempo_PointListComponent::getNumRows()
{
    if(!Polytempo_Composition::getInstance()->getSelectedSequence()) return 0;
    return Polytempo_Composition::getInstance()->getSelectedSequence()->getControlPoints()->size();
}

void Polytempo_PointListComponent::selectedRowsChanged(int /*lastRowSelected*/)
{
    Polytempo_Composition* composition = Polytempo_Composition::getInstance();
    
    composition->clearSelectedControlPointIndices();
    SparseSet<int> indices = table->getSelectedRows();
    for(int i=0;i<indices.size();i++)
    {
        composition->addSelectedControlPointIndex(indices[i]);
    }
    composition->updateContent();
}

void Polytempo_PointListComponent::paintCell(Graphics& /*g*/, int /*rowNumber*/, int /*columnId*/, int /*width*/, int /*height*/, bool /*rowIsSelected*/)
{}

Component* Polytempo_PointListComponent::refreshComponentForCell(int rowNumber, int columnId, bool rowIsSelected, Component* existingComponentToUpdate)
{
    if(columnId == 5)
    {
        auto* checkbox = static_cast<CheckboxTableCell*> (existingComponentToUpdate);
        // If an existing component is being passed-in for updating, we'll re-use it, but
        // if not, we'll have to create one.
        if (checkbox == nullptr)
            checkbox = new CheckboxTableCell(this);

        checkbox->setState(getText(rowNumber, columnId) == "1");
        checkbox->setRowAndColumn (rowNumber, columnId);
        return checkbox;
    }

    auto* textLabel = static_cast<EditableTextCustomComponent*> (existingComponentToUpdate);
    // If an existing component is being passed-in for updating, we'll re-use it,
    // but if not, we'll have to create one.
    if (textLabel == nullptr)
        textLabel = new EditableTextCustomComponent(this);
    
    // start points: no tempo in
    else if(columnId == 3 && getText(rowNumber, 5) == "1")
    {
        textLabel->setEditable(false);
        textLabel->setColour(Label::textColourId, Colour(0xffaaaaaa));
        textLabel->setText("--", dontSendNotification);
    }
    // last point or point before start point: no tempo out
    else if(columnId == 4 &&
            (rowNumber == getNumRows()-1 ||
             (rowNumber < getNumRows()-1 && getText(rowNumber+1, 5) == "1")))
    {
        textLabel->setEditable(false);
        textLabel->setColour(Label::textColourId, Colour(0xffaaaaaa));
        textLabel->setText("--", dontSendNotification);
    }
    else
    {
        textLabel->setEditable(false, true, false);
        textLabel->setFont(Font (13.0f, rowIsSelected ? Font::bold : Font::plain));
        textLabel->setColour(Label::textColourId, rowIsSelected ? Colour(0,0,0) : Colour(90,90,90));
        textLabel->setText(getText(rowNumber, columnId), dontSendNotification);
    }

 
    textLabel->setRowAndColumn(rowNumber, columnId);
    
    return textLabel;
}

void Polytempo_PointListComponent::showPopupMenu()
{
    ApplicationCommandManager* commandManager = &Polytempo_ComposerApplication::getCommandManager();
    
    PopupMenu m;
    
    m.addCommandItem(commandManager, Polytempo_CommandIDs::insertControlPoint);
    m.addCommandItem(commandManager, Polytempo_CommandIDs::removeControlPoints);
    m.addCommandItem(commandManager, Polytempo_CommandIDs::shiftControlPoints);
    m.addCommandItem(commandManager, Polytempo_CommandIDs::adjustControlPoints);
    m.addCommandItem(commandManager, Polytempo_CommandIDs::adjustTempo);

    m.show();
}
