cmake_minimum_required(VERSION 3.12)

set(CMAKE_OSX_DEPLOYMENT_TARGET "10.9" CACHE STRING "Minimum OSX deployment version")

project(Polytempo)

# Disable MSVC pragma warning
if ( MSVC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /wd4068")
endif()

add_subdirectory(JUCE)
add_subdirectory("POLYTEMPO NETWORK")
add_subdirectory("POLYTEMPO COMPOSER")