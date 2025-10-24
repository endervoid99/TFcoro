# Compiler settings are inherited from parent project
# But you can override them here if needed:
# set(CMAKE_C_COMPILER clang)
# set(CMAKE_CXX_COMPILER clang++)

# Get all source files except platform-specific ones
file(GLOB CPPCORO_SOURCES lib/*.cpp lib/**/*.cpp)

# Remove Windows-specific files on non-Windows platforms
if(NOT WIN32)
    # List of Windows-dependent files to exclude on Linux
    set(WINDOWS_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/win32.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/file.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/file_read_operation.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/file_write_operation.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/read_only_file.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/read_write_file.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/write_only_file.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/readable_file.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/writable_file.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/socket.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/socket_accept_operation.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/socket_connect_operation.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/socket_disconnect_operation.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/socket_recv_operation.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/socket_recv_from_operation.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/socket_send_operation.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/socket_send_to_operation.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/io_service.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/auto_reset_event.cpp
    )
    
    list(REMOVE_ITEM CPPCORO_SOURCES ${WINDOWS_FILES})
    
    # Print what files we're excluding for debugging
    message(STATUS "Excluding Windows-dependent cppcoro files on Linux:")
    foreach(FILE ${WINDOWS_FILES})
        if(EXISTS ${FILE})
            message(STATUS "  - ${FILE}")
        endif()
    endforeach()
endif()

add_library(libcppcoro ${CPPCORO_SOURCES})
add_library(libcppcoro::cppcoro ALIAS libcppcoro)
file(GLOB CPPCORO_HEADERS include/cppcoro/*.hpp include/cppcoro/**/*.hpp)
target_sources(libcppcoro INTERFACE ${CPPCORO_HEADERS})
target_include_directories(libcppcoro PUBLIC include)

# Set C++ standard for cppcoro (inherit from parent or set explicitly)
target_compile_features(libcppcoro PUBLIC cxx_std_23)

include_directories(include)


# Link Windows system libraries on Windows platform
if(WIN32)
    target_link_libraries(libcppcoro PUBLIC synchronization kernel32 ws2_32 mswsock)
endif()