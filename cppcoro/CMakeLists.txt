# Compiler settings are inherited from parent project
# But you can override them here if needed:
# set(CMAKE_C_COMPILER clang)
# set(CMAKE_CXX_COMPILER clang++)

file(GLOB CPPCORO_SOURCES lib/*.cpp lib/**/*.cpp)
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