cmake_minimum_required(VERSION 3.7)

set(NAME 3D_Engine)

project(${NAME})
set(CMAKE_CXX_STANDARD 11)


INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include/)
file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.c*")
add_executable(${NAME} ${SOURCES} src/core_engine/component/mesh/animated_mesh.cpp src/core_engine/component/mesh/animated_mesh.h src/rendering_engine/model/joint.cpp src/rendering_engine/model/joint.h)

IF(APPLE)
    macro(ADD_FRAMEWORK fwname)
        TARGET_LINK_LIBRARIES(${NAME} ${CMAKE_SOURCE_DIR}/frameworks/${fwname}.framework)
    endmacro(ADD_FRAMEWORK)

    add_framework(SDL2)
    add_framework(OpenGL)
    add_framework(OpenAL)
    add_framework(GLEW)
ENDIF (APPLE)