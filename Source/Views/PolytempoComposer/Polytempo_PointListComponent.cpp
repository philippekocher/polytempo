/* ==============================================================================
 
 This file is part of the POLYTEMPO software package.
 Copyright (c) 2016 - Zurich University of the Arts,
 ICST Institute for Computer Music and Sound Technology
 http://www.icst.net
 
 Author: Philippe Kocher
 
 POLYTEMPO is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 POLYTEMPO is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this software. If not, see <http://www.gnu.org/licenses/>.
 
 ============================================================================== */

#include "../JuceLibraryCode/JuceHeader.h"
#include "../../Application/PolytempoComposer/Polytempo_ComposerApplication.h"
#include "Polytempo_PointListComponent.h"
#include "../../Data/Polytempo_Composition.h"
#include "../../Application/Polytempo_CommandIDs.h"
#include "../../Misc/Polytempo_TempoMeasurement.h"


Polytempo_PointListComponent::Polytempo_PointListComponent()
{
    addAndMakeVisible(table);
    table.setModel(this);
    table.setMultipleSelectionEnabled(true);
    
                                        // id, width, min, max, flags
    table.getHeader().addColumn("Time",       1, 50, 50, -1, TableHeaderComponent::visible);
    table.getHeader().addColumn("Position",   2, 50, 50, -1, TableHeaderComponent::visible);
    table.getHeader().addColumn("Tempo In",   3, 50, 50, -1, TableHeaderComponent::visible);
    table.getHeader().addColumn("Tempo Out",  4, 50, 50, -1, TableHeaderComponent::visible);
    table.getHeader().addColumn("Start",      5, 50, 40, 40, TableHeaderComponent::visible);
    table.getHeader().addColumn("Cue",        6, 50, 30, 30, TableHeaderComponent::visible);
    
    table.getHeader().setPopupMenuActive(false);
    table.getHeader().setStretchToFitActive(true);
    
    table.setColour(ListBox::backgroundColourId, Colour(245,245,245));
}

Polytempo_PointListComponent::~Polytempo_PointListComponent()
{
}

void Polytempo_PointListComponent::paint(Graphics& g)
{
    Polytempo_Sequence* sequence = Polytempo_Composition::getInstance()->getSelectedSequence();
    if(sequence == nullptr) return;
    
    g.fillAll(sequence->getColour());
    
    table.updateContent();

    Polytempo_Composition* composition = Polytempo_Composition::getInstance();
    SparseSet<int>* indices = new SparseSet<int>();
    for(int index : *composition->getSelectedControlPointIndices())
    {
        indices->addRange(Range<int>(index,index+1));
    }
    table.setSelectedRows(*indices, dontSendNotification);
}


String Polytempo_PointListComponent::getText(int rowNumber, int columnId)
{
    Polytempo_ControlPoint* controlPoint = Polytempo_Composition::getInstance()->getSelectedSequence()->getControlPoint(rowNumber);
    String text;
    
    switch(columnId)
    {
        case 1:
            text = String(controlPoint->time);
            break;
        case 2:
            text = (controlPoint->position).toString();
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
            text = String(controlPoint->cueIn);
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
            sequence->setControlPointPosition(rowNumber, text.getFloatValue(), -1);
            break;
        case 2:
            sequence->setControlPointPosition(rowNumber, -1, Rational(text));
            break;
        case 3:
            sequence->setControlPointTempos(rowNumber, Polytempo_TempoMeasurement::encodeTempoFromUI(text.getFloatValue()), -1);
            break;
        case 4:
            sequence->setControlPointTempos(rowNumber, -1, Polytempo_TempoMeasurement::encodeTempoFromUI(text.getFloatValue()));
            break;
        case 5:
            sequence->setControlPointStartAndCue(rowNumber, text.getIntValue(), -1);
            break;
        case 6:
            sequence->setControlPointStartAndCue(rowNumber, -1, text.getIntValue());
            break;
    }

    Polytempo_Composition::getInstance()->updateContent(); // repaint everything
}

int Polytempo_PointListComponent::getNumRows()
{
    if(!Polytempo_Composition::getInstance()->getSelectedSequence()) return 0;
    return Polytempo_Composition::getInstance()->getSelectedSequence()->getControlPoints()->size();
}

void Polytempo_PointListComponent::selectedRowsChanged(int lastRowSelected)
{
    Polytempo_Composition* composition = Polytempo_Composition::getInstance();
    
    composition->clearSelectedControlPointIndices();
    SparseSet<int> indices = table.getSelectedRows();
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
        textLabel->setEditable(true);
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

    m.addSeparator();
    
    m.addCommandItem(commandManager, Polytempo_CommandIDs::adjustTime);
    m.addCommandItem(commandManager, Polytempo_CommandIDs::adjustPosition);
    m.addCommandItem(commandManager, Polytempo_CommandIDs::adjustTempo);

    m.show();
}
