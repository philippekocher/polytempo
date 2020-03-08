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

#ifndef __Polytempo_CommandIDs__
#define __Polytempo_CommandIDs__


namespace Polytempo_CommandIDs
{
    enum
    {
        open               = 0x2000,
        clearOpenRecent,
        newDocument,
        save,
        saveAs,
        close,
        exportSelected, exportAll,
        startStop,
        markerFwd, markerBwd,
        imageFwd, imageBwd,
        returnToLoc,
        returnToBeginning,
        gotoTime,
        aboutWindow,
        preferencesWindow,
        fullScreen,
        help,
        visitWebsite,
        
        showMainView,

        // network
        zoomIn, zoomOut,
 
        showPageEditor,
        showRegionEditor,
        showScoreEditor,

        loadImage,
        addSection,
        addInstance,
        addRegion,
        deleteSelected,

        
        // composer
        zoomInX, zoomOutX,
        zoomInY, zoomOutY,
        
        showGraphicExportView,
        showTimeMap,
        showTempoMap,
        showPointList,
        showPatternList,
        
        playbackSettingsWindow,
  
        addSequence,
        removeSequence,
        insertControlPoint,
        removeControlPoints,
        shiftControlPoints,
        adjustTime,
        adjustPosition,
        adjustControlPoints,
        adjustTempo,
        
        addBeatPattern,
        insertBeatPattern,
        removeBeatPattern,
        
		annotationAccept,
		annotationCancel,
		annotationDelete,
		annotationLayerSettings,
		annotationOff,
		annotationReadOnly,
		annotationStandard,
		annotationEdit,

        openRecent // must be last!
    };
}

#endif  // __Polytempo_CommandIDs__
