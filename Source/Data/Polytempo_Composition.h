#pragma once

#include "JuceHeader.h"
#include "Polytempo_Score.h"
#include "Polytempo_Sequence.h"
#include "../Misc/Polytempo_Alerts.h"


class Polytempo_Composition
{
public:
    Polytempo_Composition();
    ~Polytempo_Composition();

    juce_DeclareSingleton(Polytempo_Composition, false)
    
    void setMainWindow(DocumentWindow*);
    void updateContent();
    
    void setDirty(bool flag);
    bool isDirty();
    
    void addSequence();
    void removeSequence(int);
    int  getNumberOfSequences();
    
    Polytempo_Sequence* getSequence(int);
    Polytempo_Sequence* getSelectedSequence();
    Polytempo_Sequence* getSequenceWithID(int);

    bool isOneSequenceSoloed();

    void setSelectedSequenceIndex(int);
    int getSelectedSequenceIndex();
    
    void clearSelectedControlPointIndices();
    void addSelectedControlPointIndex(int);
    void removeSelectedControlPointIndex(int);
    void setSelectedControlPointIndex(int);
    Array<int>* getSelectedControlPointIndices();
    bool isSelectedControlPointIndex(int);

    void updateScore();
    void findCoincidingControlPoints();
    
    void unsavedChangesAlert(Polytempo_YesNoCancelAlert::callbackTag tag);
    void newComposition();
    void openFile(File file = File());
    void saveToFile(bool showFileDialog);
    void writeJSONtoFile(File file);
    bool readJSONfromFile(File file);

    void exportSelectedSequence();
    void exportAllSequences();
    void exportAsPlainText();
    void exportAsLispList();
    void exportAsCArray();
    void exportAsPolytempoScore();

private:
    bool scoreNeedsUpdate = false;
    int selectedSequenceIndex = -1;
    Array<int> *selectedControlPointIndices;
    bool exportAll;
    
    DocumentWindow *mainWindow;
    OwnedArray <Polytempo_Sequence> sequences;

    std::unique_ptr<Polytempo_Score> score;
    
    File compositionFile = File();
    File shouldOpenFile;
    bool dirty = false;
};
