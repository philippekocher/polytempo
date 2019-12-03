#pragma once

namespace Polytempo_CommandIDs
{
    enum
    {
        open = 0x2000,
        clearOpenRecent,
        newDocument,
        save,
        saveAs,
        close,
        exportSelected,
        exportAll,
        startStop,
        markerFwd,
        markerBwd,
        imageFwd,
        imageBwd,
        returnToLoc,
        returnToBeginning,
        gotoTime,
        aboutWindow,
        preferencesWindow,
        fullScreen,
        help,
        visitWebsite,

        // network
        zoomIn,
        zoomOut,

        showMainView,
        showPageEditor,
        showRegionEditor,
        showScoreEditor,

        loadImage,
        addSection,
        addInstance,
        addRegion,
        deleteSelected,

        // composer
        zoomInX,
        zoomOutX,
        zoomInY,
        zoomOutY,

        showTimeMap,
        showTempoMap,
        showPointList,
        showPatternList,

        playbackSettingsWindow,

        addSequence,
        removeSequence,
        insertControlPoint,
        removeControlPoint,
        adjustTime,
        adjustPosition,
        adjustTempo,
        alignWithCursor,

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
