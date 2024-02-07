#Checks for cmake build in assimp
[ -d "dependencies/assimp/bin/libassimp.so.5.3.0" ] || cd "dependencies/assimp" && cmake CMakeLists.txt && cmake --build . && cd "../../"

#Checks for build folder
[ -d "build/" ] ||  mkdir "build/"

#Checks for cmake build 
[ -d "build/CMakeCache.txt" ] || cmake CMakeLists.txt -B build/ -G "Unix Makefiles"

cd "build/" && make && cd ".." && ./build/3d-engine
