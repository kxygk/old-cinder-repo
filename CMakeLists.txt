cmake_minimum_required(VERSION 3.0)
project(buubletea)
#set(CMAKE_CXX_COMPILER /usr/bin/g++-5)

find_package(Git REQUIRED)

    message(STATUS "!!! Build type")
    message(STATUS ${CMAKE_BUILD_TYPE})

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(build-folder build-release)
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(build-folder build-debug)
else()
    set(build-folder build-unknown)
endif()
    
add_subdirectory(external)

find_package(OpenCV PATHS ${OpenCV_BUILD_DIR} NO_DEFAULT_PATH REQUIRED)
find_package(JPEG REQUIRED)

add_subdirectory(src)

# OpenCV Example
add_executable(testProgram src/main.cpp)
target_include_directories(testProgram PUBLIC ${OpenCV_INCLUDE_DIRS})
target_link_libraries(testProgram ${OpenCV_LIBS})

# Boost (GIL) example (requires libjpg)
add_executable(mandelbrot src/mandelbrot.cpp)
target_include_directories(mandelbrot PUBLIC ${BOOST_INCLUDE_DIR})
target_include_directories(mandelbrot PUBLIC ${JPEG_INCLUDE_DIR})
target_link_libraries(mandelbrot ${JPEG_LIBRARIES})
set_property(TARGET mandelbrot PROPERTY CXX_STANDARD 11)

# Cinder examples
get_filename_component( CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external/Cinder/src" ABSOLUTE )
get_filename_component( APP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/" ABSOLUTE )
include( "${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake" )

# - Basic App
ci_make_app(
    APP_NAME    BasicApp
	SOURCES     ${APP_PATH}/src/BasicApp.cpp
	CINDER_PATH ${CINDER_PATH}
)


# - OpenGL tutorial
ci_make_app(
    APP_NAME    OpenGL-Tutorial
	SOURCES     ${APP_PATH}/src/OpenGL-Tutorial.cpp
	CINDER_PATH ${CINDER_PATH}
)

# - Geometry
ci_make_app(
    APP_NAME    GeometryApp
	SOURCES     ${APP_PATH}/src/GeometryApp.cpp
	CINDER_PATH ${CINDER_PATH}
)

# - Cinder-UI
#file( GLOB CINDERUI_SRC_FILES ${CINDER_PATH}/blocks/CinderUI/src/*.cpp)
#message(STATUS "%%%%% Configuring ${SRC_FILES}")

# set( SRC_FILES 
#         ${APP_PATH}/src/CinderUI.cpp 
#         ${CINDER_BLOCK_SOURCE_CinderUI}
# )
ci_make_app(
    APP_NAME    CinderUI
    SOURCES     ${APP_PATH}/src/CinderUI.cpp 
	CINDER_PATH ${CINDER_PATH}
    BLOCKS      UI
)

ci_make_app(
    APP_NAME    OpenGL-Tutorial-in-Cinder
    SOURCES     ${APP_PATH}/src/OpenGL-Tutorial-in-Cinder.cpp 
	CINDER_PATH ${CINDER_PATH}
    BLOCKS      UI
)

