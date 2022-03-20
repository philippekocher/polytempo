# Copyright 2018 The Min-DevKit Authors. All rights reserved.
# Use of this source code is governed by the MIT License found in the License.md file.

cmake_minimum_required(VERSION 3.0)

set(C74_MIN_API_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../min-api)
include(${C74_MIN_API_DIR}/script/min-pretarget.cmake)


#############################################################
# MAX EXTERNAL
#############################################################


include_directories( 
	"${C74_INCLUDES}"
)


set( SOURCE_FILES
	${PROJECT_NAME}.cpp
)


add_library( 
	${PROJECT_NAME} 
	MODULE
	${SOURCE_FILES}
)

include(${C74_MIN_API_DIR}/script/min-posttarget.cmake)


IF(APPLE)
   #SET(GUI_TYPE MACOSX_BUNDLE)
   INCLUDE_DIRECTORIES ( /Developer/Headers/FlatCarbon )
   FIND_LIBRARY(CARBON_LIBRARY Carbon)
   FIND_LIBRARY(COCOA_LIBRARY Cocoa)
   MARK_AS_ADVANCED (CARBON_LIBRARY COCOA_LIBRARY)
   SET(EXTRA_LIBS ${CARBON_LIBRARY} ${COCOA_LIBRARY})

   target_link_libraries(${PROJECT_NAME} 
     PUBLIC debug "${CMAKE_CURRENT_SOURCE_DIR}/../../../../POLYTEMPO LIB/bin/libTimeSyncLib.a" ${EXTRA_LIBS} 
     PUBLIC optimized "${CMAKE_CURRENT_SOURCE_DIR}/../../../../POLYTEMPO LIB/bin/Release/libTimeSyncLib.a" ${EXTRA_LIBS}
   )
ENDIF (APPLE)

IF(MSVC)
   target_link_libraries(${PROJECT_NAME} 
     PUBLIC debug "${CMAKE_CURRENT_SOURCE_DIR}/../../../../POLYTEMPO LIB/bin/Static Library/TimeSyncLib.lib"
     PUBLIC optimized "${CMAKE_CURRENT_SOURCE_DIR}/../../../../POLYTEMPO LIB/bin/Release/Static Library/TimeSyncLib.lib"
   )
ENDIF(MSVC)


#############################################################
# UNIT TEST
#############################################################

#include(${C74_MIN_API_DIR}/test/min-object-unittest.cmake)
