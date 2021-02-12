# Include dirs
set(AGENT_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${VENDOR}/websocketpp
    ${VENDOR}/asio/asio/include
)

# Header files
set(AGENT_HEADERS
    "pch.h"
    "agent.hpp"
    "core/logger.h"
    "debug/assert.h"
    "debug/instrumentor.h"
)

# Source files
set(AGENT_SOURCES
    "main.cpp"
    "core/logger.cpp"
)

# Application executable
add_executable(agent ${AGENT_SOURCES})

# Check for MSVC
if(MSVC)
    # Check for dynamic runtime
    if(USE_MSVC_DYNAMIC_LINKED_RUNTIME)
        set_target_properties(agent PROPERTIES
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
        )
    else()
        set_target_properties(agent PROPERTIES
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
        )
    endif()
endif()

# Definitions
target_compile_definitions(agent
PUBLIC
    # Asio
    ASIO_STANDALONE
    # WebSocketpp
    _WEBSOCKETPP_CPP11_FUNCTIONAL_
    _WEBSOCKETPP_CPP11_SYSTEM_ERROR_
    _WEBSOCKETPP_CPP11_RANDOM_DEVICE_
    _WEBSOCKETPP_CPP11_MEMORY_
    _WEBSOCKETPP_CPP11_STL_
    # MSVC
    UNICODE
    _UNICODE
    # Debug Build
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Debug>:ENABLE_LOGGER>
    $<$<CONFIG:Debug>:ENABLE_PROFILING>
    $<$<CONFIG:Debug>:ENABLE_ASSERTIONS>
    # Release With Debug Info
    $<$<CONFIG:RelWithDebInfo>:ENABLE_LOGGER>
    $<$<CONFIG:RelWithDebInfo>:ENABLE_PROFILING>
    $<$<CONFIG:RelWithDebInfo>:ENABLE_ASSERTIONS>
    # Release Build
    $<$<CONFIG:Release>:NDEBUG>
)

# Setting the pre compiled header
target_precompile_headers(agent PUBLIC
    "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/pch.hpp>"
)

# Application properties
set_target_properties(agent
PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED 17
)

# Applicaiton include dirs
target_include_directories(agent
PUBLIC
    ${AGENT_INCLUDE_DIRS}
)

# Application libraries
target_link_libraries(agent
PUBLIC
    spdlog::spdlog_header_only
    clipp::clipp
    nlohmann_json::nlohmann_json
)

# Setting custom commandos to copy all needed files to the right location
add_custom_target(copy_resources_agent ALL
    COMMAND cmake -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/bin/assets
    COMMAND cmake -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets ${CMAKE_SOURCE_DIR}/bin/assets
    DEPENDS agent)

# Setting the install command to the app
install(TARGETS agent RUNTIME DESTINATION bin)
install(TARGETS agent LIBRARY DESTINATION lib)
