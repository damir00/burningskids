cmake_minimum_required(VERSION 2.6)
project(DUserClientTest)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-write-strings") # get rid of chipmunk warnings

INCLUDE_DIRECTORIES(/usr/include/GL /usr/include/IL)
INCLUDE_DIRECTORIES(../)

link_directories (../)

add_executable(test test.cpp)

target_link_libraries(test duserclient GL GLU glut IL ILU ILUT)

