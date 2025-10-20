file(GLOB CPPCORO_SOURCES lib/*.cpp lib/**/*.cpp)
add_library(libcppcoro ${CPPCORO_SOURCES})
add_library(libcppcoro::cppcoro ALIAS libcppcoro)
file(GLOB CPPCORO_HEADERS include/cppcoro/*.hpp include/cppcoro/**/*.hpp)
target_sources(libcppcoro INTERFACE ${CPPCORO_HEADERS})
target_include_directories(libcppcoro PUBLIC include)

include_directories(include)


# Link Windows system libraries on Windows platform
if(WIN32)
    target_link_libraries(libcppcoro PUBLIC synchronization kernel32 ws2_32 mswsock)
endif()