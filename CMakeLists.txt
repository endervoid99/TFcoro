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

include_directories(cppcoro/include)

file(GLOB TFCORO_SOURCES src/*.cpp src/**/*.cpp)
add_library(libtfcoro ${TFCORO_SOURCES})
add_library(libtfcoro::libtfcoro ALIAS libtfcoro)
file(GLOB TFCORO_HEADERS include/*.hpp include/**/*.hpp)
target_sources(libtfcoro INTERFACE ${TFCORO_HEADERS})
target_include_directories(libtfcoro PUBLIC include)
target_include_directories(libtfcoro PUBLIC cppcoro/include)

add_subdirectory(cppcoro)
target_link_libraries(libtfcoro PRIVATE libcppcoro::cppcoro)

#test main


file(GLOB TESTMAIN_SOURCES src/*.cpp src/**/*.cpp testbed/main.cpp)
add_executable(testmain ${TESTMAIN_SOURCES})
file(GLOB TFCORO_HEADERS include/*.hpp include/**/*.hpp)
target_sources(testmain INTERFACE ${TFCORO_HEADERS})
target_include_directories(testmain PUBLIC include)
target_include_directories(testmain PUBLIC cppcoro/include)

target_link_libraries(testmain PRIVATE libcppcoro::cppcoro)
target_link_libraries(testmain PRIVATE libtfcoro::libtfcoro)

include_directories(cppcoro/include)
include_directories(include)

