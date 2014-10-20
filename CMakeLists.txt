cmake_minimum_required(VERSION 2.6)
project(R2DGame)

include(CheckIncludeFiles)

cmake_policy(SET CMP0015 OLD)	# relative link paths

#set(CMAKE_CXX_COMPILER "g++")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-write-strings") # get rid of chipmunk warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wreturn-type")

set(CMAKE_BUILD_TYPE Debug)

link_directories(r2dengine/libs)
add_library(imp_chipmunk STATIC IMPORTED)

if (UNIX AND NOT WIN32)
	if (CMAKE_SIZEOF_VOID_P MATCHES "8")
		message("Build with x86_64 chipmunk")
		set_property(TARGET imp_chipmunk PROPERTY IMPORTED_LOCATION
			${CMAKE_CURRENT_SOURCE_DIR}/r2dengine/libs/64bit/libchipmunk.a)

	else (CMAKE_SIZEOF_VOID_P MATCHES "8")
		message("Build with x86 chipmunk")
		set_property(TARGET imp_chipmunk PROPERTY IMPORTED_LOCATION
			${CMAKE_CURRENT_SOURCE_DIR}/r2dengine/libs/32bit/libchipmunk.a)

	endif (CMAKE_SIZEOF_VOID_P MATCHES "8")
else (UNIX AND NOT WIN32)
	message("We have no clue what the hell is this pc. Fallback to x86_64 chipmunk")
	set_property(TARGET imp_chipmunk PROPERTY IMPORTED_LOCATION
		${CMAKE_CURRENT_SOURCE_DIR}/r2dengine/libs/64bit/libchipmunk.a)

endif (UNIX AND NOT WIN32)

add_subdirectory(duserclient)
add_subdirectory(r2dengine)

###############
# LUA version #
###############
set(LUA_INCLUDE /usr/include/lua5.1)
set(LUA_LIBS lua5.1)

CHECK_INCLUDE_FILES(lua.h HAVE_LUA_H)
if (HAVE_LUA_H)
	unset(LUA_INCLUDE)
	set(LUA_LIBS lua)
endif (HAVE_LUA_H)
### LUA

include_directories(
	duserclient
	duserclient/deps
	r2dengine
	dioengine
	r2dengine/include/chipmunk
	/usr/include/GL
	/usr/include/IL
	/usr/include/libxml2
	${LUA_INCLUDE}
)
	
add_executable(r2d 
	r2d.cpp 
	gametrack.cpp 
	gamecar.cpp
	gameaudio.cpp
	dtimelimiter.cpp
	gameutils.cpp 
#	gameobject.cpp
	gamegraphic.cpp
	gameroad.cpp
	gameresourcemanager.cpp
	luatools.cpp
	luacarcontroller.cpp
	luaapi.cpp
	xmlparser.cpp
	gamelevel.cpp
	gamemenu.cpp
	gamewidget.cpp
	gamecampaign.cpp
	gamecampaigndefinition.cpp
	propinterface.cpp
	gamegraph.cpp
	dquadtree.cpp
	gamescript.cpp
)

#add_executable(audiotest
#	gameaudiotest.cpp
#	#gameaudio.cpp
#)
#
#target_link_libraries(audiotest
#    openal
#    alut
#    vorbis
#)

target_link_libraries(r2d
	duserclient
	imp_chipmunk
	r2dengine
	GLEW
	GL
	GLU
	glut
	IL
	ILU
	ILUT
	openal
	alut
	${LUA_LIBS}
	luabind
	xml2
	vorbisfile
)


