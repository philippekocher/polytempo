cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

# set the project name
project(TimeSyncLibConsoleApp)

# add the executable
add_executable(TimeSyncLibConsoleApp main.cpp)

target_link_libraries(TimeSyncLibConsoleApp PUBLIC "${CMAKE_SOURCE_DIR}/../libs/Static Library/TimeSyncLib.lib")
