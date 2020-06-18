# parallel-amp-demos

Purpose of this repository is demonstation of parallel computing on C++.

This solution consist of 4 projects:
 - AMPIntro (Сomparsion of different tools)
 - GameOfLife (Conway's Game of Life with computation on GPU)
 - HeatMap (Temperature distribution simulation)
 - Test (couple of unit tests)

Used technologies:
 - OpenMP (CPU multithreading)
 - AMP C++ (GPU computing)
 - Google Test
 - SFML (GUI)


## Build
1. Restore NuGet packages (for Test project)
2. Download SFML (tested on version 2.5.1)
3. Copy folder "SFML-2.5.1" to "packages" folder
4. Build and run

Currently only supported build with Visual Studio (tested on Visual C++ 15)
SFML is linked statically.
You can debug GPU code only when your accelerator supports debugging, otherwise debug run will throw exception.
