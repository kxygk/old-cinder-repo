include(build_external_project.cmake)

message(STATUS "****** Managing external projects in:")
message(STATUS "~> ${CMAKE_CURRENT_LIST_DIR}")

set(EXTERNAL_PROJECT_NAME "OpenCV")
set(EXTERNAL_PROJECT_FOLDER ${CMAKE_CURRENT_LIST_DIR}/${EXTERNAL_PROJECT_NAME})
# build_external_project_url(
#     ${EXTERNAL_PROJECT_NAME}
#     ${EXTERNAL_PROJECT_FOLDER}
#     https://github.com/opencv/opencv/archive/3.1.0.zip
#     ${build-folder}
#     "make -j4"
#     -DBUILD_EXAMPLES=OFF
#     -DBUILD_PERF_TESTS=OFF
#     -DBUILD_TESTS=OFF
#     -DBUILD_opencv_python2=OFF
#     -DWITH_CUDA=OFF
#     -DWITH_CUFFT=OFF
#     -DWITH_GTK=OFF
#     -DWITH_IPP=OFF
#     -DWITH_MATLAB=OFF
#     -DWITH_QT=ON
#     -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE})

build_external_project_git(
    ${EXTERNAL_PROJECT_NAME}
    ${EXTERNAL_PROJECT_FOLDER}
    https://github.com/opencv/opencv.git
    3.1.0
    ${build-folder}
    "make -j4"
    -DBUILD_EXAMPLES=OFF
    -DBUILD_PERF_TESTS=OFF
    -DBUILD_TESTS=OFF
    -DBUILD_opencv_python2=OFF
    -DWITH_CUDA=OFF
    -DWITH_CUFFT=OFF
    -DWITH_GTK=OFF
    -DWITH_IPP=OFF
    -DWITH_MATLAB=OFF
    -DWITH_QT=ON
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE})

set(${EXTERNAL_PROJECT_NAME}_BUILD_DIR ${EXTERNAL_PROJECT_FOLDER}/${build-folder} PARENT_SCOPE)



set(EXTERNAL_PROJECT_NAME "Boost")
set(EXTERNAL_PROJECT_FOLDER ${CMAKE_CURRENT_LIST_DIR}/${EXTERNAL_PROJECT_NAME})
header_only_external_project_url(
    ${EXTERNAL_PROJECT_NAME}
    ${EXTERNAL_PROJECT_FOLDER}
    http://downloads.sourceforge.net/project/boost/boost/1.62.0/boost_1_62_0.zip)
set(${EXTERNAL_PROJECT_NAME}_INCLUDE_DIR ${EXTERNAL_PROJECT_FOLDER}/src PARENT_SCOPE)



set(EXTERNAL_PROJECT_NAME "Cinder")
set(EXTERNAL_PROJECT_FOLDER ${CMAKE_CURRENT_LIST_DIR}/${EXTERNAL_PROJECT_NAME})
build_external_project_git(
    ${EXTERNAL_PROJECT_NAME}
    ${EXTERNAL_PROJECT_FOLDER}
    https://github.com/cinder/Cinder.git
    android_linux
    ${build-folder}
    "make -j4")

set(${EXTERNAL_PROJECT_NAME}_BUILD_DIR ${EXTERNAL_PROJECT_FOLDER}/${build-folder} PARENT_SCOPE)
