# This function is used to force a build on a dependant project at cmake configuration phase.
# 
function ( build_external_project_url
    PROJ                    # Project
    PROJ_FOLDER             # Project Location
    PROJ_URL                # Where to get project
    PROJ_BUILD_FOLDER_NAME) # Build folder based on build typeforce_${PROJ}

    set(trigger_build_dir ${PROJ_FOLDER}/cmake-trigger-directory)

    #mktemp dir in build tree
    file(MAKE_DIRECTORY ${trigger_build_dir} ${trigger_build_dir}/build)

    #generate false dependency project
    set(CMAKE_LIST_CONTENT "
    cmake_minimum_required(VERSION 3.2.1)

    include(ExternalProject)
    ExternalProject_add(${PROJ}
        PREFIX              ${PROJ_FOLDER}
        STAMP_DIR           ${PROJ_FOLDER}/stamp
        SOURCE_DIR          ${PROJ_FOLDER}/src
        URL                 ${PROJ_URL}
        DOWNLOAD_DIR        ${PROJ_FOLDER}/download-zip
        BINARY_DIR          ${PROJ_FOLDER}/${PROJ_BUILD_FOLDER_NAME}
        INSTALL_COMMAND \"\"
        CMAKE_ARGS ${ARGN}
        )

        add_custom_target(trigger_${PROJ})
        add_dependencies(trigger_${PROJ} ${PROJ})")

    message(STATUS "%%%%% Configation:")
    message(STATUS ">>>>> ${CMAKE_LIST_CONTENT} <<<<<")
    file(WRITE ${trigger_build_dir}/CMakeLists.txt "${CMAKE_LIST_CONTENT}")

    message(STATUS "%%%%% Configuring ${PROJ}")
    execute_process(COMMAND ${CMAKE_COMMAND} ..
        WORKING_DIRECTORY ${trigger_build_dir}/build
        )
    message(STATUS "%%%%% Building ${PROJ}")
    execute_process(COMMAND ${CMAKE_COMMAND} --build .
        WORKING_DIRECTORY ${trigger_build_dir}/build
        )

endfunction()


function ( build_external_project_git
    PROJ                    # Project
    PROJ_FOLDER             # Project Location
    PROJ_GIT_REPOSITORY
    PROJ_GIT_TAG
    PROJ_BUILD_FOLDER_NAME) # Build folder based on build type

    set(trigger_build_dir ${PROJ_FOLDER}/build-temp)

    #mktemp dir in build tree
    file(MAKE_DIRECTORY ${trigger_build_dir} ${trigger_build_dir}/build)

    #generate false dependency project
    set(CMAKE_LIST_CONTENT "
    cmake_minimum_required(VERSION 3.2.1)

    include(ExternalProject)
    ExternalProject_add(${PROJ}
        PREFIX              ${PROJ_FOLDER}
        STAMP_DIR           ${PROJ_FOLDER}/stamp
        SOURCE_DIR          ${PROJ_FOLDER}/src
        GIT_REPOSITORY      ${PROJ_GIT_REPOSITORY}
        GIT_TAG             ${PROJ_GIT_TAG}
        DOWNLOAD_DIR        ${PROJ_FOLDER}/download-git
        BINARY_DIR          ${PROJ_FOLDER}/${PROJ_BUILD_FOLDER_NAME}
        INSTALL_COMMAND \"\"
        CMAKE_ARGS ${ARGN}
        )

        add_custom_target(trigger_${PROJ})
        add_dependencies(trigger_${PROJ} ${PROJ})")

#    message(STATUS "%%%%% Configation:")
#    message(STATUS ">>>>> ${CMAKE_LIST_CONTENT} <<<<<")
    file(WRITE ${trigger_build_dir}/CMakeLists.txt "${CMAKE_LIST_CONTENT}")

    message(STATUS "%%%%% Configuring ${PROJ}")
    execute_process(COMMAND ${CMAKE_COMMAND} ..
        WORKING_DIRECTORY ${trigger_build_dir}/build
        )
    message(STATUS "%%%%% Building ${PROJ}")
    execute_process(COMMAND ${CMAKE_COMMAND} --build .
        WORKING_DIRECTORY ${trigger_build_dir}/build
        )

endfunction()
