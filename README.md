# Marching Cubes

Implementation of marching cubes algorithm in C++ .

## How To Run This On Windows
* Clone the repository including the submodules using : 
```properties
git clone --recurse-submodules https://github.com/The-curs0r/marching-cubes.git
```
* Go into the cloned repository and run CMake : 
```properties
cd marching-cubes
cmake CMakeLists.txt -B ./build/ -G "Visual Studio 16 2019"
cd build
start marchingCubes.sln
```
* Build the solution for Release configuration
* Copy the shader files to the Release folder and run the application :   
```properties
copy *.glsl ./Release/
cd Release
start .\marchingCubes.exe
```

## How To Run This On Linux
* Clone the repository including the submodules using : 
```properties
git clone --recurse-submodules https://github.com/The-curs0r/marching-cubes.git
```
* Go into the cloned repository and run CMake : 
```properties
cd marching-cubes
cmake CMakeLists.txt -B ./build/ -G "Unix Makefiles"
cd build
make all
./marchingCubes 
```