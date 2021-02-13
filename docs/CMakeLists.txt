# Find Doxigen and require dot, treat the other components as optional
find_package(Doxygen)
option(MIDDLEWARE_BUILD_DOCUMENTATION "Build middleware documentation (requires Doxygen)" ${DOXYGEN_FOUND})


if(MIDDLEWARE_BUILD_DOCUMENTATION)
    if(NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen is needed to build the documentation.")
    endif()

    # Find all the public headers
    get_target_property(MIDDLEWARE_PUBLIC_HEADER_DIR middleware INTERFACE_INCLUDE_DIRECTORIES)
    file(GLOB_RECURSE MIDDLEWARE_PUBLIC_HEADERS
        ${MIDDLEWARE_PUBLIC_HEADER_DIR}/*.h
        ${MIDDLEWARE_PUBLIC_HEADER_DIR}/*.hpp
    )

    get_target_property(AGENT_PUBLIC_HEADER_DIR agent INTERFACE_INCLUDE_DIRECTORIES)
    file(GLOB_RECURSE AGENT_PUBLIC_HEADERS
        ${AGENT_PUBLIC_HEADER_DIR}/*.h
        ${AGENT_PUBLIC_HEADER_DIR}/*.hpp
    )

    get_target_property(CLIENT_PUBLIC_HEADER_DIR client INTERFACE_INCLUDE_DIRECTORIES)
    file(GLOB_RECURSE CLIENT_PUBLIC_HEADERS
        ${CLIENT_PUBLIC_HEADER_DIR}/*.h
        ${CLIENT_PUBLIC_HEADER_DIR}/*.hpp
    )

    # This will be the main output of our command
    set(DOXYGEN_INPUT_DIR "${PROJECT_SOURCE_DIR}/middleware ${PROJECT_SOURCE_DIR}/agent")
    set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/docs/doxygen)
    set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/html/index.html)
    set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
    set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    # Doxygen variables
    set(DOXYGEN_MAIN_PAGE_MDFILE ${PROJECT_SOURCE_DIR}/README.md)
    set(DOXYGEN_EXAMPLES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/examples)
    set(DOXYGEN_IMAGES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/images)

    # Replace variables inside @@ with the current values
    configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

    file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR}) # Doxygen won't create this for us
    add_custom_command(OUTPUT ${DOXYGEN_INDEX_FILE}
                    DEPENDS ${MIDDLEWARE_PUBLIC_HEADERS} ${AGENT_PUBLIC_HEADERS} ${CLIENT_PUBLIC_HEADERS}
                    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
                    MAIN_DEPENDENCY ${DOXYFILE_OUT} ${DOXYFILE_IN}
                    COMMENT "Generating docs")

    add_custom_target(Doxygen ALL DEPENDS ${DOXYGEN_INDEX_FILE})

endif()
