cmake_minimum_required(VERSION 4.0.0)

# Uncomment these lines if you want to set compiler in CMakeLists.txt instead of presets
# set(CMAKE_C_COMPILER clang)
# set(CMAKE_CXX_COMPILER clang++)

project (TFCoro VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

#build runtime directories are not ../Debug or ../Release (VS2022). Still doesnt work from visual studio, cant find executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")


#set(CMAKE_CXX_COMPILER_CLANG_SCAN_DEPS "/usr/lib/llvm-18/bin/clang-scan-deps")

if(CMAKE_BUILD_TYPE STREQUAL "Shipping" OR CMAKE_CONFIGURATION_TYPES MATCHES "Shipping")
    add_compile_definitions(SHIPPING_BUILD)
endif()

# Build cppcoro (Windows-specific files are excluded in cppcoro/CMakeLists.txt)
include_directories(cppcoro/include)
add_subdirectory(cppcoro)

# Find source files (excluding testbed)
file(GLOB_RECURSE TFCORO_SOURCES src/*.cpp)
list(FILTER TFCORO_SOURCES EXCLUDE REGEX ".*/testbed/.*")

# Only create library if we have source files
if(TFCORO_SOURCES)
    add_library(libtfcoro ${TFCORO_SOURCES})
    add_library(libtfcoro::libtfcoro ALIAS libtfcoro)
    file(GLOB TFCORO_HEADERS include/*.hpp include/**/*.hpp)
    target_sources(libtfcoro INTERFACE ${TFCORO_HEADERS})
    target_include_directories(libtfcoro PUBLIC include)

    target_include_directories(libtfcoro PUBLIC cppcoro/include)
    target_link_libraries(libtfcoro PRIVATE libcppcoro::cppcoro)
else()
    # Create a header-only interface library
    add_library(libtfcoro INTERFACE)
    add_library(libtfcoro::libtfcoro ALIAS libtfcoro)
    file(GLOB TFCORO_HEADERS include/*.h include/*.hpp include/**/*.h include/**/*.hpp)
    target_sources(libtfcoro INTERFACE ${TFCORO_HEADERS})
    target_include_directories(libtfcoro INTERFACE include)

    target_include_directories(libtfcoro INTERFACE cppcoro/include)
    target_link_libraries(libtfcoro INTERFACE libcppcoro::cppcoro)
endif()

#test main
add_executable(testmain src/testbed/main.cpp)
file(GLOB TFCORO_HEADERS include/*.h include/*.hpp include/**/*.h include/**/*.hpp)
target_sources(testmain INTERFACE ${TFCORO_HEADERS})
target_include_directories(testmain PUBLIC include)

target_include_directories(testmain PUBLIC cppcoro/include)
target_link_libraries(testmain PRIVATE libcppcoro::cppcoro)

target_link_libraries(testmain PRIVATE libtfcoro::libtfcoro)

include_directories(include)

