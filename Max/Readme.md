# Polytempo Max External

## Prerequisites

To be able to compile the Max-External using CMake, the „POLYTEMPO LIB“ project has to be compiled manually, using Projucer and a build environment (Xcode or Visual Studio) first.

## Compile

Using CMake, the external can be compiled as follows:

    cd {path to polytempo/Max folder}
    mkdir build
    cd build
    cmake ..
    cmake --build . --config Release

### Additional Information for OSX

As default, the architecture (Intel or ARM) of the build machine is used. If your build system allows cross-compilation, you can build a fat binary using the following CMake command (instead of cmake ..):

    cmake .. -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64"
 