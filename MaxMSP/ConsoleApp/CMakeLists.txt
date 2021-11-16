cmake_minimum_required(VERSION 3.10)

if (MSVC)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif (MSVC)


# set the project name
project(TimeSyncLibConsoleApp)


IF(APPLE)
   #SET(GUI_TYPE MACOSX_BUNDLE)
   INCLUDE_DIRECTORIES ( /Developer/Headers/FlatCarbon )
   FIND_LIBRARY(CARBON_LIBRARY Carbon)
   FIND_LIBRARY(COCOA_LIBRARY Cocoa)
   MARK_AS_ADVANCED (CARBON_LIBRARY COCOA_LIBRARY)
   SET(EXTRA_LIBS ${CARBON_LIBRARY} ${COCOA_LIBRARY})
ENDIF (APPLE)

# add the executable
add_executable(TimeSyncLibConsoleApp main.cpp)

target_link_libraries(TimeSyncLibConsoleApp PUBLIC "${CMAKE_SOURCE_DIR}/../libs/libTimeSyncLib.a" ${EXTRA_LIBS})
