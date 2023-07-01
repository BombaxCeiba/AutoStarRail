# 清零前缀，后续手动指定
include(AsrResetLibPrefix)

add_library(AsrCoreObjects OBJECT)
target_compile_options(AsrCoreObjects PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)
set_property(TARGET AsrCoreObjects PROPERTY POSITION_INDENDENT_CODE ON)
target_compile_definitions(AsrCoreObjects PRIVATE -DASR_BUILD_SHARED)
target_link_libraries(AsrCoreObjects PUBLIC
    Boost::filesystem
    ICU::data
    ICU::i18n
    ICU::uc)

if(ASR_USE_BUNDLIED_BOOST)
    target_link_libraries(AsrCoreObjects PUBLIC ${ASR_BUNDLIED_BOOST_LIBS})
else()
    # 对于使用msys包管理器安装boost的用户，使用msys的clangd或使用这个语句强制将Boost_INCLUDE_DIRS添加到compile_commands.json
    # target_compile_options(AsrCoreObjects PRIVATE "-I${Boost_INCLUDE_DIRS}")
    target_include_directories(AsrCoreObjects PUBLIC ${Boost_INCLUDE_DIRS})
    target_link_libraries(AsrCoreObjects PRIVATE ${Boost_LIBRARIES})
endif()

if(WIN32)
    target_compile_definitions(AsrCoreObjects PRIVATE ASR_PLATFORM=WINDOWS)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    target_compile_definitions(AsrCoreObjects PRIVATE ASR_PLATFORM=MACOS)
elseif(UNIX)
    target_compile_definitions(AsrCoreObjects PRIVATE ASR_PLATFORM=LINUX)
else()
    message(FATAL_ERROR "Unsupported platform.")
endif()

# asr_add_core_conpoment(Utils)
asr_add_core_conpoment(Impl)
asr_add_core_conpoment(ForeignInterfaceHost)
asr_add_core_conpoment(GlobalSettingsManager)
asr_add_core_conpoment(Logger)
asr_add_core_conpoment(Exceptions)
asr_add_core_conpoment(i18n)

add_library(libAsrCore SHARED)
add_dependencies(libAsrCore AsrCoreObjects)

if(${BUILD_TEST})
    add_library(AsrStaticCore STATIC)
    add_dependencies(AsrStaticCore libAsrCore)
endif()

add_compile_definitions(-DASR_BUILD_SHARED)

# 配置导出库
set(ASR_EXPORT_MACRO_LIST "")
set(ASR_FOREIGN_LANGUAGE_LIB_LIST "")

cmake_policy(SET CMP0078 NEW)

file(GLOB ASR_SWIG_FILES ${CMAKE_SOURCE_DIR}/SWIG/*.i)
set_property(SOURCE ${ASR_SWIG_FILES} PROPERTY CPLUSPLUS ON)
set_source_files_properties(${ASR_SWIG_FILES} PROPERTIES USE_SWIG_DEPENDENCIES TRUE)

if(${EXPORT_CSHARP})
    asr_add_swig_export_library(CSharp libAsrCoreCSharpExport "${ASR_SWIG_FILES}")
    target_link_libraries(libAsrCoreCSharpExport PUBLIC libAsrCore)
    add_dependencies(libAsrCoreCSharpExport libAsrCore)
    list(APPEND ASR_EXPORT_MACRO_LIST ASR_EXPORT_CSHARP)
endif()

if(${EXPORT_JAVA})
    find_package(JNI REQUIRED)
    asr_add_swig_export_library(Java libAsrCoreJavaExport "${ASR_SWIG_FILES}")
    set_property(TARGET libAsrCoreJavaExport PROPERTY SWIG_COMPILE_OPTIONS -package org.asr)

    target_link_libraries(AsrCoreObjects PUBLIC JNI::JNI)

    target_link_libraries(libAsrCoreJavaExport PUBLIC libAsrCore)
    add_dependencies(libAsrCoreJavaExport libAsrCore)
    list(APPEND ASR_EXPORT_MACRO_LIST ASR_EXPORT_JAVA)
endif()

if(${EXPORT_PYTHON})
    find_package(Python3 COMPONENTS Development REQUIRED)
    asr_add_swig_export_library(Python libAsrCorePythonExport "${ASR_SWIG_FILES}")

    target_include_directories(AsrCoreObjects PUBLIC ${Python3_INCLUDE_DIRS})
    target_link_libraries(AsrCoreObjects PUBLIC ${Python3_LIBRARIES})

    target_link_libraries(libAsrCorePythonExport PUBLIC libAsrCore)
    add_dependencies(libAsrCorePythonExport libAsrCore)
    list(APPEND ASR_EXPORT_MACRO_LIST ASR_EXPORT_PYTHON)
endif()

cmake_policy(SET CMP0135 NEW)

# FetchContent_Declare(
# Sol2
# URL ${GITHUB_MIRROR_URL}/ThePhD/sol2/archive/refs/tags/v3.3.0.zip
# )
# FetchContent_MakeAvailable(Sol2)
# target_link_libraries(AsrCoreObjects PUBLIC sol2::sol2)
# find_package(Lua 5.4)

# if(NOT ${LUA_FOUND})
# message(STATUS "Downloading and configuring Lua 5.4.6.")
# FetchContent_Declare(
# AsrLua
# URL ${GITHUB_MIRROR_URL}/lua/lua/archive/refs/tags/v5.4.6.zip
# PATCH_COMMAND ${CMAKE_COMMAND} -E copy
# "${CMAKE_SOURCE_DIR}/cmake/CompileLua.cmake" <SOURCE_DIR>/CMakeLists.txt
# )
# FetchContent_MakeAvailable(AsrLua)
# target_link_libraries(AsrCoreObjects PUBLIC Asr::Lua)
# else()
# target_include_directories(AsrCoreObjects PRIVATE ${LUA_INCLUDE_DIR})
# target_link_libraries(AsrCoreObjects PUBLIC ${LUA_LIBRARIES})
# endif()

# list(APPEND ASR_EXPORT_MACRO_LIST ASR_EXPORT_LUA)
target_link_libraries(AsrCoreObjects
    PUBLIC AsrUtils ${ASR_FOREIGN_LANGUAGE_LIB_LIST})
target_compile_definitions(AsrCoreObjects PUBLIC ${ASR_EXPORT_MACRO_LIST})

target_link_libraries(libAsrCore PUBLIC AsrCoreObjects)

if(${BUILD_TEST})
    target_link_libraries(AsrStaticCore PUBLIC AsrCoreObjects)
    asr_add_core_test(ForeignInterfaceHost ASR_EX_PRIVATE_LIBS)
endif()
