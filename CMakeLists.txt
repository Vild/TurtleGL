cmake_minimum_required(VERSION 2.8)
project(turtlegl)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# Compile options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O0 -Wall")
if(WIN32)
	include_directories("${PROJECT_SOURCE_DIR}/windows/include")
	set(SDL2_PATH "${PROJECT_SOURCE_DIR}/windows")
	set(CMAKE_INCLUDE_PATH "${PROJECT_SOURCE_DIR}/windows/include")
	set(CMAKE_LIBRARY_PATH "${PROJECT_SOURCE_DIR}/windows/lib")
	set(CMAKE_PREFIX_PATH "${PROJECT_SOURCE_DIR}/windows")
endif()

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

find_package(SDL2_image REQUIRED)
include_directories(${SDL2_IMAGE_INCLUDE_DIR})

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})

if(WIN32)
	set(ASSIMP_LIBRARIES "${CMAKE_LIBRARY_PATH}/assimp-vc140-mt.lib")
else()
	set(ASSIMP_LIBRARIES assimp)
endif()

include_directories(${PROJECT_SOURCE_DIR}/src)

# Source files
file(GLOB_RECURSE SOURCE_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HEADER_FILES ${PROJECT_SOURCE_DIR}/src/*.hpp)

# Compile and Link
add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARY} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${ASSIMP_LIBRARIES})

if(WIN32)
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/windows/bin"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>)
endif()
