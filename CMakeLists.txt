cmake_minimum_required(VERSION 3.7)
project(Game_Engine)
set(CMAKE_CXX_STANDARD 11)

macro(ADD_FRAMEWORK fwname appname)
    find_library(FRAMEWORK_${fwname}
            NAMES ${fwname}
            PATHS ${CMAKE_SOURCE_DIR}/frameworks
            PATH_SUFFIXES Frameworks
            NO_DEFAULT_PATH)
    if (${FRAMEWORK_${fwname}} STREQUAL FRAMEWORK_${fwname}-NOTFOUND)
        MESSAGE(ERROR ": Framework ${fwname} not found")
    else()
        TARGET_LINK_LIBRARIES(${appname} ${FRAMEWORK_${fwname}})
        MESSAGE(STATUS "Framework ${fwname} found at ${FRAMEWORK_${fwname}}")
    endif()
endmacro(ADD_FRAMEWORK)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include/)
file(GLOB_RECURSE SOURCES "src/*.c*")
add_executable(Game_Engine ${SOURCES})

IF(APPLE)
    add_framework(SDL2 Game_Engine)
    add_framework(OpenGL Game_Engine)
    add_framework(OpenAL Game_Engine)
    add_framework(GLEW Game_Engine)
ENDIF (APPLE)