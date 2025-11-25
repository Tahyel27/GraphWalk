# Graph Walk

## compilation

**Easiest:** use the cmake VSCode extension (there is an o), open the project in the root directory and launch the cmake configure command, on the side pannel go to the cmake tab and in project status/configure change debug to release for high performance. Then just run the cmake build command. There is an official guide on how to install c++ compilers and cmake on windows for use with VSCode https://code.visualstudio.com/docs/cpp/config-mingw
### Linux

1. Make sure to have cmake and a c++ compiler installed.

2. Go to the project root directory and create a build directory (Like build release)

3. In the build directory run ```cmake -DCMAKE_BUILD_TYPE=Release ..```
4. Compile by running ```cmake --build .```

### Windows

1. Make sure to have a c++ compiler installed, I recommend installing it through MSYS2 https://www.msys2.org/ you can also use visual studio

2. Go to the project root directory and create a build directory (Like build release)
2.1. at this point you could try importing the project into visual studio, I never did this so idk if it will work

3. in the build directory run either
3.1. if you have MinGW installed ```cmake -DCMAKE_BUILD_TYPE=Release .. -G "MinGW Makefiles"``` 
3.2. if you have visual studio ```cmake -DCMAKE_BUILD_TYPE=Release .. -G "Visual Studio 17 2022"``` *I never tested this, so it may yell at you*
4. Compile
4.1. if you have MinGW run ```cmake --build .```
4.2. if you have visual studio open the solution file and proceed from there as normal