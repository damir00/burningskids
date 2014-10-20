cmake_minimum_required(VERSION 2.6)
project(R2DEngine)

#set(CMAKE_CXX_COMPILER "i586-mingw32msvc-g++")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-write-strings") # get rid of chipmunk warnings

INCLUDE_DIRECTORIES(include/chipmunk)

add_library(r2dengine r2dlib.cpp r2dsim.cpp r2dsurface.cpp r2dcar.cpp
	r2dengine.cpp r2dtyre.cpp r2dobject.cpp r2dlerper.cpp)

target_link_libraries(r2dengine imp_chipmunk)
