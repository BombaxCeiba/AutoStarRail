cmake_minimum_required(VERSION 3.21)

project(AtuoStarRail LANGUAGES CXX VERSION 0.1)

# fix missing CMAKE_ASM_MASM_COMPILE_OBJECT or CMAKE_ASM_NASM_COMPILE_OBJECT
# NOTICE: I have modified the CMakeLists.txt in boost context, so that boost can explicitly use MASM on Windows.
#         Then I MUST enable ASM_NASM after ASM_MASM, therefore CMake will compile asm files in OpenCV using NASM compiler as defualt asm complier.
if(CMAKE_ASM_MASM_COMPILER)
    enable_language(ASM_MASM)
endif()
if(CMAKE_ASM_NASM_COMPILER)
    enable_language(ASM_NASM)
endif()

option(EXPORT_JAVA "导出Java接口" OFF)
option(EXPORT_CSHARP "导出C#接口" OFF)
option(EXPORT_PYTHON "导出Python接口" OFF)

set(ASR_SPD_FMT_OPTION_EXPLANATION "项目自带的spdlog使用标准库的fmt，OFF则会使用spdlog自带的fmt")
# See [C++ 20 Compiler support](https://en.cppreference.com/w/cpp/compiler_support/20)
if((${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" AND ${CMAKE_CXX_COMPILER_VERSION} VERSION_GREATER 17) OR
    (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" AND ${CMAKE_CXX_COMPILER_VERSION} VERSION_GREATER 13) OR
    (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC" AND ${CMAKE_CXX_COMPILER_VERSION} VERSION_GREATER 19.29))
    option(ASR_SPDLOG_USE_STD_FMT ${ASR_SPD_FMT_OPTION_EXPLANATION} ON)
    option(ASR_USE_BUNDLIED_FMT "" OFF)
else()
    option(ASR_SPDLOG_USE_STD_FMT ${ASR_SPD_FMT_OPTION_EXPLANATION} OFF)
    option(ASR_USE_BUNDLIED_FMT "" ON)
endif()

option(BUILD_TEST "" OFF)
option(BUILD_QT5_GUI "" ON)
option(USE_LLD "Using lld instead of using ld." OFF)
option(ASR_USE_CCACHE "Using CCACHE" OFF)

option(ASR_USE_BUNDLIED_EIGEN "强制使用下载的Eigen源码构建OpenCV" OFF)
option(ASR_USE_BUNDLIED_BOOST "强制使用下载的Boost源码构建" OFF)
option(ASR_USE_BUNDLIED_OPENCV "强制使用下载的OpenCV源码构建" OFF)

if(NOT DEFINED GITHUB_MIRROR_URL)
    set(GITHUB_MIRROR_URL "https://github.com")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(ASR_DEFAULT_SHARED_LIBRARY_PREFIX ${CMAKE_SHARED_LIBRARY_PREFIX})
set(ASR_DEFAULT_STATIC_LIBRARY_PREFIX ${CMAKE_STATIC_LIBRARY_PREFIX})
set(ASR_DEFAULT_SHARED_LIBRARY_PREFIX_CXX ${CMAKE_SHARED_LIBRARY_PREFIX_CXX})
set(ASR_DEFAULT_STATIC_LIBRARY_PREFIX_CXX ${CMAKE_STATIC_LIBRARY_PREFIX_CXX})
set(ASR_DEFAULT_SHARED_LIBRARY_PREFIX_C ${CMAKE_SHARED_LIBRARY_PREFIX_C})
set(ASR_DEFAULT_STATIC_LIBRARY_PREFIX_C ${CMAKE_STATIC_LIBRARY_PREFIX_C})

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