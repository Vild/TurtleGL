project(turtlegl)

cmake_minimum_required(VERSION 2.8)

# Utils
macro(LIST_REPLACE LIST INDEX NEWVALUE)
    list(INSERT ${LIST} ${INDEX} ${NEWVALUE})
    MATH(EXPR __INDEX "${INDEX} + 1")
    list (REMOVE_AT ${LIST} ${__INDEX})
endmacro(LIST_REPLACE)

# Compile options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O0 -Wall -Werror")

INCLUDE(FindPkgConfig)

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
PKG_SEARCH_MODULE(OpenGL REQUIRED gl)
PKG_SEARCH_MODULE(GLEW REQUIRED glew)

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR} ${GLEW_INCLUDE_DIRS})

# Source files
#src/main.cpp src/engine.cpp src/scopeexit.cpp src/shader.cpp src/mesh.cpp
file(STRINGS "Sources.txt" SOURCE_FILES)
separate_arguments(SOURCE_FILES)

string(REGEX REPLACE "([^;]+)" "src/\\1.cpp" SOURCE_FILES "${SOURCE_FILES}")

# Compile and Link
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES})
