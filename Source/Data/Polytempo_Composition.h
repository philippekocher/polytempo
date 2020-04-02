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

#ifndef __Polytempo_Composition__
#define __Polytempo_Composition__

#include "../../JuceLibraryCode/JuceHeader.h"
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

    bool isOneSequenceSoloed();

    void setSelectedSequenceIndex(int);
    int getSelectedSequenceIndex();
    
    void clearSelectedControlPointIndices();
    void addSelectedControlPointIndex(int);
    void removeSelectedControlPointIndex(int);
    Array<int>* getSelectedControlPointIndices();
    bool isSelectedControlPointIndex(int);

    void updateScore();
    void findCoincidingControlPoints();
    
    void unsavedChangesAlert(Polytempo_YesNoCancelAlert::callbackTag tag);
    void newComposition();
    void openFile();
    void openFile(File file);
    void saveToFile();
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
    Component *mainComponent;
    OwnedArray <Polytempo_Sequence> sequences;

    std::unique_ptr<Polytempo_Score> score;
    
    File compositionFile = File();
    bool dirty = false;
};


#endif  // __Polytempo_Composition__
