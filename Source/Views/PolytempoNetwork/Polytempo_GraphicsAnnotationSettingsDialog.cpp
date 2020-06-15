#include "JuceHeader.h"
#include "Polytempo_GraphicsAnnotationSettingsDialog.h"
#include "Polytempo_GraphicsAnnotationSet.h"
#include "Polytempo_GraphicsAnnotationManager.h"

#define COLUMN_ID_SHOW 1
#define COLUMN_ID_EDIT 2

class ToggleColumnCustomComponent : public Component, Button::Listener
{
public:
    ToggleColumnCustomComponent(Polytempo_GraphicsAnnotationSettingsDialog& td) : owner(td)
    {
        addAndMakeVisible(checkBox);
        checkBox.addListener(this);
        checkBox.setWantsKeyboardFocus(false);
    }

    void resized() override
    {
        checkBox.setBoundsInset(BorderSize<int>(2));
    }

    void setRowAndColumn(int newRow, int newColumn)
    {
        row = newRow;
        columnId = newColumn;
        if (columnId == COLUMN_ID_SHOW)
            checkBox.setToggleState(owner.getShowInfo(row), dontSendNotification);
        else if (columnId == COLUMN_ID_EDIT)
            checkBox.setToggleState(owner.getEditInfo(row), dontSendNotification);

        checkBox.repaint();
    }

    void buttonClicked(Button*) override
    {
        if (columnId == COLUMN_ID_SHOW)
        {
            owner.setShowInfo(row, checkBox.getToggleState());
            setRowAndColumn(row, columnId);
        }
        else if (columnId == COLUMN_ID_EDIT)
        {
            if (!checkBox.getToggleState())
                checkBox.setToggleState(true, dontSendNotification);
            else
            {
                owner.setEditInfo(row, true);
                setRowAndColumn(row, columnId);
            }
        }
    }

private:
    Polytempo_GraphicsAnnotationSettingsDialog& owner;
    ToggleButton checkBox;
    int row, columnId;
};

class EditableTextCustomComponent : public Label
{
public:
    EditableTextCustomComponent(Polytempo_GraphicsAnnotationSettingsDialog& td) : owner(td)
    {
        setEditable(false, true, false);
    }

    void mouseDown(const MouseEvent& event) override
    {
        owner.getTable()->selectRowsBasedOnModifierKeys(row, event.mods, false);
        Label::mouseDown(event);
    }

    void textWasEdited() override
    {
        bool ok = owner.setText(row, getText());
        if (!ok)
            setText(owner.getText(row), sendNotificationAsync);
    }

    void setRowAndColumn(const int newRow, const int newColumn)
    {
        row = newRow;
        columnId = newColumn;
        setText(owner.getText(row), dontSendNotification);
    }

    void paint(Graphics& g) override
    {
        auto& lf = getLookAndFeel();
        if (!dynamic_cast<LookAndFeel_V4*>(&lf))
            lf.setColour(textColourId, Colours::black);

        Label::paint(g);
    }

private:
    Polytempo_GraphicsAnnotationSettingsDialog& owner;
    int row, columnId;
    Colour textColour;
};

//==============================================================================
Polytempo_GraphicsAnnotationSettingsDialog::Polytempo_GraphicsAnnotationSettingsDialog(OwnedArray<Polytempo_GraphicsAnnotationSet>* pAnnotationSet)
{
    this->pAnnotationSet = pAnnotationSet;

    addAndMakeVisible(table);
    table.setModel(this);
    table.getHeader().addColumn("Show", COLUMN_ID_SHOW, 45);
    table.getHeader().addColumn("Edit", COLUMN_ID_EDIT, 35);
    table.getHeader().addColumn("Name", 3, 200);
    table.getHeader().resizeAllColumnsToFit(getWidth());
    numRows = pAnnotationSet->size();

    addLayerBtn.reset(new TextButton("Add layer"));
    addAndMakeVisible(addLayerBtn.get());
    addLayerBtn->addListener(this);
}

Polytempo_GraphicsAnnotationSettingsDialog::~Polytempo_GraphicsAnnotationSettingsDialog()
{
}

void Polytempo_GraphicsAnnotationSettingsDialog::paint(Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId)); // clear the background

    g.setColour(Colours::grey);
    g.drawRect(getLocalBounds(), 1); // draw an outline around the component
}

void Polytempo_GraphicsAnnotationSettingsDialog::resized()
{
    int left = getBounds().getX() + 2;
    int width = getWidth() - 4;

    table.setBounds(left, getBounds().getY() + 2, width, getBounds().getHeight() - 20);
    addLayerBtn->setBounds(left, getHeight() - 20, width, 20);
}

int Polytempo_GraphicsAnnotationSettingsDialog::getNumRows()
{
    return numRows;
}

bool Polytempo_GraphicsAnnotationSettingsDialog::setText(const int rowNumber, String newText) const
{
    bool ok = pAnnotationSet->getUnchecked(rowNumber)->setAnnotationLayerName(newText);
    if (!ok)
    {
        AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Layer renaming", "Renaming layer failed");
        return false;
    }

    return true;
}

String Polytempo_GraphicsAnnotationSettingsDialog::getText(int row) const
{
    return pAnnotationSet->getUnchecked(row)->getAnnotationLayerName();
}

bool Polytempo_GraphicsAnnotationSettingsDialog::getShowInfo(int row) const
{
    return pAnnotationSet->getUnchecked(row)->getShow();
}

bool Polytempo_GraphicsAnnotationSettingsDialog::setShowInfo(int row, bool state)
{
    if (!state && pAnnotationSet->getUnchecked(row)->getEdit())
    {
        AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Set visibility state", "Editable layer has to be visible");
        return false;
    }

    bool ok = pAnnotationSet->getUnchecked(row)->setShow(state);
    if (!ok)
    {
        AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Set visibility state", "Setting visibility state failed");
        return false;
    }

    return true;
}

bool Polytempo_GraphicsAnnotationSettingsDialog::getEditInfo(int row) const
{
    return pAnnotationSet->getUnchecked(row)->getEdit();
}

bool Polytempo_GraphicsAnnotationSettingsDialog::setEditInfo(int row, bool state)
{
    if (!state)
        pAnnotationSet->getUnchecked(row)->setEdit(false); // unselect is not allowed
    else
    {
        // select only new layer
        for (int i = 0; i < pAnnotationSet->size(); i++)
        {
            if (i == row)
            {
                pAnnotationSet->getUnchecked(i)->setEdit(true);
                pAnnotationSet->getUnchecked(i)->setShow(true);
            }
            else
                pAnnotationSet->getUnchecked(i)->setEdit(false);
        }

        table.updateContent();
    }

    return true;
}

Component* Polytempo_GraphicsAnnotationSettingsDialog::refreshComponentForCell(int rowNumber, int columnId, bool /*isRowSelected*/,
                                                                               Component* existingComponentToUpdate)
{
    if (columnId == COLUMN_ID_SHOW || columnId == COLUMN_ID_EDIT)
    {
        ToggleColumnCustomComponent* toggleComponent = static_cast<ToggleColumnCustomComponent*>(existingComponentToUpdate);
        if (toggleComponent == nullptr)
            toggleComponent = new ToggleColumnCustomComponent(*this);

        toggleComponent->setRowAndColumn(rowNumber, columnId);
        return toggleComponent;
    }

    EditableTextCustomComponent* textLabel = static_cast<EditableTextCustomComponent*>(existingComponentToUpdate);
    if (textLabel == nullptr)
        textLabel = new EditableTextCustomComponent(*this);

    textLabel->setRowAndColumn(rowNumber, columnId);
    return textLabel;
}

void Polytempo_GraphicsAnnotationSettingsDialog::paintRowBackground(Graphics& g, int rowNumber, int /*width*/, int /*height*/, bool rowIsSelected)
{
    const Colour alternateColour(getLookAndFeel().findColour(ListBox::backgroundColourId)
                                                 .interpolatedWith(getLookAndFeel().findColour(ListBox::textColourId), 0.03f));
    if (rowIsSelected)
        g.fillAll(Colours::lightblue);
    else if (rowNumber % 2)
        g.fillAll(alternateColour);
}

void Polytempo_GraphicsAnnotationSettingsDialog::paintCell(Graphics& g, int /*rowNumber*/, int /*columnId*/,
                                                           int width, int height, bool /*rowIsSelected*/)
{
    g.setColour(getLookAndFeel().findColour(ListBox::textColourId));
    g.setColour(getLookAndFeel().findColour(ListBox::backgroundColourId));
    g.fillRect(width - 1, 0, 1, height);
}

void Polytempo_GraphicsAnnotationSettingsDialog::buttonClicked(Button* btn)
{
    if (btn == addLayerBtn.get())
    {
        Polytempo_GraphicsAnnotationManager::getInstance()->createAndAddNewLayer(false);
        numRows = pAnnotationSet->size();
        table.updateContent();
    }
}

void Polytempo_GraphicsAnnotationSettingsDialog::show(OwnedArray<Polytempo_GraphicsAnnotationSet>* pAnnotationSet)
{
    Polytempo_GraphicsAnnotationSettingsDialog* p = new Polytempo_GraphicsAnnotationSettingsDialog(pAnnotationSet);
    p->setSize(300, 390);

    DialogWindow::LaunchOptions options;
    options.content.setOwned(p);
    options.dialogTitle = "Annotation settings";
    options.dialogBackgroundColour = Colours::white;
    options.escapeKeyTriggersCloseButton = true;
    options.resizable = false;
#if defined JUCE_ANDROID || defined JUCE_IOS
	options.useNativeTitleBar = false;
#else
    options.useNativeTitleBar = true;
#endif
    options.launchAsync();
}

TableListBox* Polytempo_GraphicsAnnotationSettingsDialog::getTable()
{
    return &table;
}
