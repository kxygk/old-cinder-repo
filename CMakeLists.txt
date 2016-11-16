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


add_executable(testProgram src/main.cpp)
target_include_directories(testProgram PUBLIC ${OpenCV_INCLUDE_DIRS})
target_link_libraries(testProgram ${OpenCV_LIBS})

add_executable(mandelbrot src/mandelbrot.cpp)
target_include_directories(mandelbrot PUBLIC ${BOOST_INCLUDE_DIR})
target_include_directories(mandelbrot PUBLIC ${JPEG_INCLUDE_DIR})
target_link_libraries(mandelbrot ${JPEG_LIBRARIES})
set_property(TARGET mandelbrot PROPERTY CXX_STANDARD 11)




