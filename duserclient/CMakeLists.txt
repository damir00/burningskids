cmake_minimum_required(VERSION 2.6)
project(DUserClient)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-write-strings") # get rid of chipmunk warnings

add_subdirectory(deps/glm)

INCLUDE_DIRECTORIES(
	/usr/include/GL
	/usr/include/IL
	deps
)
add_library(duserclient 
	dvect.cpp
	dvect3.cpp
	duserclientobject.cpp
	duserclient.cpp
	duserclientfreeglut.cpp
	dparticle.cpp
	dresource.cpp
	dutils.cpp
	drendernode.cpp
	dglfbo.cpp
	dglshader.cpp
	duserclientgeometry.cpp
	dboundingbox.cpp
)

