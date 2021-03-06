cmake_minimum_required(VERSION 3.9)
project(gl_library)

set(CMAKE_C_STANDARD 99)
add_definitions(-g -Wall -x c)


add_library(carthage
        src/debug.c
        src/debug.h
        src/fifo.c
        src/fifo.h
        src/list.c
        src/list.h
        src/log.c
        src/log.h
        src/map.c
        src/map.h
        src/sscanner.c
        src/sscanner.h
        src/sublime_term.c
        src/sublime_term.h
        src/thread_lib.c
        src/thread_lib.h
        src/vector.c
        src/vector.h
        carthage.h
)

if (WIN32)
    add_executable(test_exec
            src/debug.c
            src/debug.h
            src/fifo.c
            src/fifo.h
            src/list.c
            src/list.h
            src/log.c
            src/log.h
            src/map.c
            src/map.h
            src/sscanner.c
            src/sscanner.h
            src/sublime_term.c
            src/sublime_term.h
            src/thread_lib.c
            src/thread_lib.h
            src/vector.c
            src/vector.h
            carthage.h
            test/library_test_windows.c)
    message(STATUS "OS >>> WINDOWS")
endif ()

if (UNIX)
    add_executable(test_exec
            src/debug.c
            src/debug.h
            src/fifo.c
            src/fifo.h
            src/list.c
            src/list.h
            src/log.c
            src/log.h
            src/map.c
            src/map.h
            src/sscanner.c
            src/sscanner.h
            src/sublime_term.c
            src/sublime_term.h
            src/thread_lib.c
            src/thread_lib.h
            src/vector.c
            src/vector.h
            carthage.h
            test/cunit.c
            test/cunit.h
            test/library_test_linux.c)
    message(STATUS "OS >>> UNIX")
endif ()


target_link_libraries(carthage pthread)
target_link_libraries(test_exec pthread)

