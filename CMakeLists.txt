cmake_minimum_required(VERSION 3.10)
project(measuring_fragmentation)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(USE_JEMALLOC "Use jemalloc allocator" ON)

if(USE_JEMALLOC)
    if(WIN32 OR MINGW)
        # Manually specify jemalloc include\\lib paths on Windows\\MinGW
        set(JEMALLOC_INCLUDE_DIR "C:\\msys64\\home\\AniDashyan\\jemalloc\\include")
        set(JEMALLOC_LIBRARY "C:\\msys64\\home\\AniDashyan\\jemalloc\\lib\\jemalloc.lib")

        if(EXISTS "${JEMALLOC_INCLUDE_DIR}\\jemalloc\\jemalloc.h")
            message(STATUS "Using jemalloc from ${JEMALLOC_LIBRARY}")
            include_directories(${JEMALLOC_INCLUDE_DIR})
            add_compile_definitions(USE_JEMALLOC)
            set(EXTRA_LIBS ${EXTRA_LIBS} ${JEMALLOC_LIBRARY})
        else()
            message("jemalloc not found at specified location!")
        endif()
    else()
        # On Linux/macOS, use pkg-config to find jemalloc
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(JEMALLOC REQUIRED jemalloc)
        if(JEMALLOC_FOUND)
            message(STATUS "Found jemalloc via pkg-config")
            include_directories(${JEMALLOC_INCLUDE_DIRS})
            add_compile_definitions(USE_JEMALLOC)
            set(EXTRA_LIBS ${EXTRA_LIBS} ${JEMALLOC_LIBRARIES})
        else()
            message("jemalloc not found via pkg-config!")
        endif()
    endif()
endif()

add_executable(frag_test main.cpp)
target_link_libraries(frag_test ${EXTRA_LIBS})
