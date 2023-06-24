cmake_minimum_required(VERSION 3.21)

project(AtuoStarRail LANGUAGES CXX VERSION 0.1)


option(EXPORT_JAVA "导出Java接口" OFF)
option(EXPORT_CSHARP "导出C#接口" OFF)
option(EXPORT_PYTHON "导出Python接口" OFF)

option(ASR_SPDLOG_USE_STD_FMT "项目自带的spdlog使用标准库的fmt，OFF则会使用spdlog自带的fmt" ON)

option(BUILD_TEST "" OFF)
option(BUILD_QT5_GUI "" ON)
option(USE_LLD "Using lld instead of using ld." OFF)


if(NOT DEFINED GITHUB_MIRROR_URL)
    set(GITHUB_MIRROR_URL "https://github.com")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(AsrUtils)

include(AsrSetLibPrefix)
include(AsrSetOutputPath)

include(3rdParty/CMakeLists.txt)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(${ASR_USE_CCACHE})
    find_program(CCACHE "ccache")

    if(CCACHE)
        set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE})
        set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
    endif()
endif()

if(WIN32)
    add_definitions(-DNOMINMAX -D_USE_MATH_DEFINES)
endif()

if(MSVC)
    add_compile_options(/Zc:__cplusplus /utf-8)
endif()

if(${USE_LLD} AND NOT MSVC)
    find_program(
        LLD_FOUND
        lld REQUIRED)
    add_compile_options("-fuse-ld=lld")
endif()

add_custom_target(RemoveSwigOutput
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_SOURCE_DIR}/cmake/AsrRemoveSwigOutput.cmake)

if(${BUILD_TEST})
    enable_testing()
endif()

add_subdirectory(AutoStarRail)