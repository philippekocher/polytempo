#pragma once

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
        checkForNewVersion,
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
        adjustControlPoints,
        adjustTempo,
        moveLastControlPointToEnd,

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
