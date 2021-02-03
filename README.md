# Polytempo

**PolytempoNetwork** is an application to synchronise musicians. It displays the music on screen and conveys the musical tempo to the player either visually by an animation that resembles the gestures of a conductor, or acoustically by a click track.

Several computers running this software can be connected in a network. This allows for the synchronisation of ensemble music and opens up possibilities for complex tempo structures as well as the distribution of musicians in the performing space.

**PolytempoComposer** is a tool to devise temporal polyphonies. It provides a GUI for the manipulation of complex polytemporal structures and the facility to audition the tempo progressions.

For further information please refer to the [project's website](https://polytempo.zhdk.ch).


#### Clone repository

    https://github.com/philippekocher/polytempo.git

#### Checkout submodules

    git submodule update --init --recursive

#### Build

Use CMake to build PolytempoNetwork and PolytempoComposer

    cd {folder where you checked out the code}
    mkdir build
    cd build

    cmake ..
    build
