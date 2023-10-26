cmake_minimum_required(VERSION 3.22)

if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "")
    set(CMAKE_OSX_DEPLOYMENT_TARGET "13.0" CACHE STRING "Minimum OSX deployment version")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,-ld_classic" CACHE STRING "Xcode 15 Linker issue")
endif()

project(Polytempo)

# Disable MSVC pragma warning
if ( MSVC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /wd4068")
endif()
if ( CMAKE_COMPILER_IS_GNUCC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas -Wno-overloaded-virtual")
endif()

add_subdirectory(JUCE)
add_subdirectory("POLYTEMPO NETWORK")
add_subdirectory("POLYTEMPO COMPOSER")

