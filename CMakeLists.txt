cmake_minimum_required(VERSION 4.0.0)

# Uncomment these lines if you want to set compiler in CMakeLists.txt instead of presets
# set(CMAKE_C_COMPILER clang)
# set(CMAKE_CXX_COMPILER clang++)

project (TFCoro VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set output directories for all configurations
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# For multi-config generators like Visual Studio, set per-config directories
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR})
endforeach()


#set(CMAKE_CXX_COMPILER_CLANG_SCAN_DEPS "/usr/lib/llvm-18/bin/clang-scan-deps")

if(CMAKE_BUILD_TYPE STREQUAL "Shipping" OR CMAKE_CONFIGURATION_TYPES MATCHES "Shipping")
    add_compile_definitions(SHIPPING_BUILD)
endif()

include_directories(cppcoro/include)

# Collect source files while preserving directory structure
file(GLOB_RECURSE TFCORO_SOURCES_ALL 
    src/*.cpp 
    src/*.hpp
    src/*.h
)

# Remove testbed files from library sources
list(FILTER TFCORO_SOURCES_ALL EXCLUDE REGEX ".*testbed.*")
set(TFCORO_SOURCES ${TFCORO_SOURCES_ALL})

# Collect header files
file(GLOB_RECURSE TFCORO_HEADERS 
    include/*.hpp 
    include/*.h
)

# Combine sources and headers for the library target
set(TFCORO_ALL_FILES ${TFCORO_SOURCES} ${TFCORO_HEADERS})

add_library(libtfcoro ${TFCORO_ALL_FILES})
add_library(libtfcoro::libtfcoro ALIAS libtfcoro)

# Create source groups for Visual Studio - use combined list
foreach(file IN LISTS TFCORO_ALL_FILES)
    get_filename_component(file_path "${file}" PATH)
    file(RELATIVE_PATH file_path_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${file_path}")
    string(REPLACE "/" "\\" group_path "${file_path_rel}")
    source_group("${group_path}" FILES "${file}")
endforeach()

target_include_directories(libtfcoro PUBLIC include)
target_include_directories(libtfcoro PUBLIC cppcoro/include)

add_subdirectory(cppcoro)
target_link_libraries(libtfcoro PRIVATE libcppcoro::cppcoro)

#test main

# Collect all sources including testbed for testmain
file(GLOB_RECURSE TESTMAIN_SOURCES_ALL 
    src/*.cpp 
    src/*.hpp
    src/*.h
)

# Also add the headers to testmain for IDE convenience
set(TESTMAIN_ALL_FILES ${TESTMAIN_SOURCES_ALL} ${TFCORO_HEADERS})

add_executable(testmain ${TESTMAIN_ALL_FILES})

# Create source groups for testmain as well
foreach(source IN LISTS TESTMAIN_ALL_FILES)
    get_filename_component(source_path "${source}" PATH)
    file(RELATIVE_PATH source_path_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${source_path}")
    string(REPLACE "/" "\\" group_path "${source_path_rel}")
    source_group("${group_path}" FILES "${source}")
endforeach()

target_include_directories(testmain PUBLIC include)
target_include_directories(testmain PUBLIC cppcoro/include)

target_link_libraries(testmain PRIVATE libcppcoro::cppcoro)
target_link_libraries(testmain PRIVATE libtfcoro::libtfcoro)

# Set working directory for Visual Studio debugging
set_target_properties(testmain PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
)

include_directories(cppcoro/include)
include_directories(include)

