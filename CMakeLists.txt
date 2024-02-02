cmake_minimum_required(VERSION 3.22)

if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Fat binary for x86 and arm architecture")
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "Minimum OSX deployment version" FORCE)
    if(XCODE_VERSION VERSION_GREATER_EQUAL 15)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-ld_classic")
        message("Xcode ${XCODE_VERSION} detected, using old linker style")
    endif()
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

