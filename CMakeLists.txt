cmake_minimum_required(VERSION 3.10)
project(measuring_fragmentation)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(USE_JEMALLOC "Use jemalloc allocator" ON)

add_executable(frag_test main.cpp)

if(USE_JEMALLOC)
    add_definitions(-DUSE_JEMALLOC)
    if(WIN32 OR MINGW)
        # Windows manual path
        set(JEMALLOC_INCLUDE_DIR "C:/msys64/home/AniDashyan/jemalloc/include")
        set(JEMALLOC_LIBRARY "C:/msys64/home/AniDashyan/jemalloc/lib/jemalloc.lib")

        if(EXISTS "${JEMALLOC_INCLUDE_DIR}/jemalloc/jemalloc.h")
            message(STATUS "Using jemalloc from ${JEMALLOC_LIBRARY}")
            target_include_directories(frag_test PRIVATE ${JEMALLOC_INCLUDE_DIR})
            target_link_libraries(frag_test PRIVATE ${JEMALLOC_LIBRARY})
            target_compile_definitions(frag_test PRIVATE USE_JEMALLOC)
        else()
            message("jemalloc not found at specified location!")
        endif()
    else()
        # Unix/Linux/macOS
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(JEMALLOC REQUIRED jemalloc)

        message(STATUS "Found jemalloc via pkg-config")
        target_include_directories(frag_test PRIVATE ${JEMALLOC_INCLUDE_DIRS})
        target_link_directories(frag_test PRIVATE ${JEMALLOC_LIBRARY_DIRS})
        target_link_libraries(frag_test PRIVATE ${JEMALLOC_LIBRARIES})
        target_compile_definitions(frag_test PRIVATE USE_JEMALLOC)
    endif()
endif()
