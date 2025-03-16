# cmake/copy_resources.cmake
# The destination directory is passed in as -DDEST_DIR=...
if(NOT DEFINED DEST_DIR)
    message(FATAL_ERROR "DEST_DIR variable not set!")
endif()

file(GLOB_RECURSE RESOURCE_FILES "${CMAKE_SOURCE_DIR}/Resources/*")
foreach(SRC_FILE IN LISTS RESOURCE_FILES)
    # Get the file’s relative path with respect to the Resources folder.
    file(RELATIVE_PATH REL_PATH "${CMAKE_SOURCE_DIR}/Resources" "${SRC_FILE}")
    set(DEST_FILE "${DEST_DIR}/${REL_PATH}")

    # Ensure the destination directory exists.
    get_filename_component(DEST_FOLDER "${DEST_FILE}" DIRECTORY)
    file(MAKE_DIRECTORY "${DEST_FOLDER}")

    if(NOT EXISTS "${DEST_FILE}")
        message(STATUS "Copying ${SRC_FILE} to ${DEST_FILE}")
        file(COPY "${SRC_FILE}" DESTINATION "${DEST_FOLDER}")
    else()
        message(STATUS "Skipping ${SRC_FILE} because ${DEST_FILE} already exists")
    endif()
endforeach()