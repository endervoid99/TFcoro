# Compiler settings are inherited from parent project
# But you can override them here if needed:
# set(CMAKE_C_COMPILER clang)
# set(CMAKE_CXX_COMPILER clang++)

# Collect source and header files while preserving directory structure
file(GLOB_RECURSE CPPCORO_SOURCES 
    lib/*.cpp 
    lib/*.hpp
    lib/*.h
)

file(GLOB_RECURSE CPPCORO_HEADERS 
    include/cppcoro/*.hpp 
    include/cppcoro/*.h
)

# Combine sources and headers for the library target
set(CPPCORO_ALL_FILES ${CPPCORO_SOURCES} ${CPPCORO_HEADERS})

add_library(libcppcoro ${CPPCORO_ALL_FILES})
add_library(libcppcoro::cppcoro ALIAS libcppcoro)

# Create source groups for Visual Studio - use combined list
foreach(file IN LISTS CPPCORO_ALL_FILES)
    get_filename_component(file_path "${file}" PATH)
    file(RELATIVE_PATH file_path_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${file_path}")
    string(REPLACE "/" "\\" group_path "${file_path_rel}")
    source_group("${group_path}" FILES "${file}")
endforeach()

target_include_directories(libcppcoro PUBLIC include)

# Set C++ standard for cppcoro (inherit from parent or set explicitly)
target_compile_features(libcppcoro PUBLIC cxx_std_23)

include_directories(include)


# Link Windows system libraries on Windows platform
if(WIN32)
    target_link_libraries(libcppcoro PUBLIC synchronization kernel32 ws2_32 mswsock)
endif()