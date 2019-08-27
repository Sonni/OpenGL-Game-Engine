cmake_minimum_required(VERSION 3.7)

set(NAME 3D_Engine)

project(${NAME})
set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS "-O0")
SET(CMAKE_C_FLAGS "-O0")

if(MSVC)
    # Force to always compile with W4
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    # Update if necessary
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")
endif()

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include/)
file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.c*")
add_executable(${NAME} ${SOURCES})

IF(APPLE)
    macro(ADD_FRAMEWORK fwname)
        TARGET_LINK_LIBRARIES(${NAME} ${CMAKE_SOURCE_DIR}/frameworks/${fwname}.framework)
    endmacro(ADD_FRAMEWORK)

    add_framework(SDL2)
    add_framework(SDL2_net)
    add_framework(OpenGL)
    add_framework(OpenAL)
    add_framework(GLEW)
ENDIF (APPLE)