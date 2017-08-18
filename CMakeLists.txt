cmake_minimum_required(VERSION 3.7)
project(Game_Engine)
set(CMAKE_CXX_STANDARD 11)

macro(ADD_FRAMEWORK fwname appname)
    TARGET_LINK_LIBRARIES(${appname} ${CMAKE_SOURCE_DIR}/frameworks/${fwname}.framework)
endmacro(ADD_FRAMEWORK)




INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include/)
file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.c*")
add_executable(Game_Engine ${SOURCES})

IF(APPLE)
    add_framework(SDL2 Game_Engine)
    add_framework(OpenGL Game_Engine)
    add_framework(OpenAL Game_Engine)
    add_framework(GLEW Game_Engine)
ENDIF (APPLE)