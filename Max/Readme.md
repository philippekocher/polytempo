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

As default, the architecture (Intel or ARM) of the build machine is used. If your build system allows cross-compilation, you can build a fat binary using the following CMake command (instead of `cmake ..`):

    cmake .. -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64"
    
On a Mac M1 machine, you will probably get the error message "polytemponetwork: cannot be loaded due to system security policy". To fix this, you need to ad-hoc code-sign the external:

    cd {path to the directory where the external is located}
    codesign --force --deep -s - polytemponetwork.mxo

### Additional Information for Windows

For the Windows build, it is required to set the package version, instead of `cmake ..`, use:

    cmake .. -DPACKAGE_VERSION=1 
