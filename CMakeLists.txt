cmake_minimum_required (VERSION 3.21)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

project (awaituv LANGUAGES CXX)


add_library(awaituv INTERFACE)
target_link_libraries(awaituv INTERFACE uv)
target_include_directories(awaituv INTERFACE inc/awaituv)

add_library(awaitcurl INTERFACE)
target_link_libraries(awaitcurl INTERFACE curl)
target_include_directories(awaitcurl INTERFACE inc/awaitcurl)

include(FetchContent)

option(ENABLE_TLSUV "Enable tlsuv support" ON)
if (ENABLE_TLSUV)
    FetchContent_Declare(tlsuv
            GIT_REPOSITORY https://github.com/kikitte/tlsuv.git
            GIT_TAG 4a09f621daecc2fe71e6184a12ecc06e974dcdd1
    )
    FetchContent_MakeAvailable(tlsuv)

    add_library(awaittlsuv INTERFACE inc/awaittlsuv/awaittlsuv.h)
    target_include_directories(awaittlsuv INTERFACE inc/awaittlsuv)
    target_link_libraries(awaittlsuv INTERFACE awaituv tlsuv)
endif ()

if (PROJECT_IS_TOP_LEVEL)
    add_subdirectory(samples)
endif ()