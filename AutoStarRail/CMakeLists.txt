add_compile_definitions($<$<CONFIG:Debug>:DEBUG>)

aux_source_directory(Utils/src ASR_UTILS_SOURCES)
add_library(AsrUtils STATIC ${ASR_UTILS_SOURCES})
set_property(TARGET AsrUtils PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories(AsrUtils PUBLIC Utils/include)
target_link_libraries(AsrUtils PUBLIC Asr3rdParty)

file(GLOB_RECURSE ASR_COMMON_HEADERS ${CMAKE_SOURCE_DIR}/include/*)
add_library(AsrCommmonHeaders INTERFACE ${ASR_COMMON_HEADERS})
target_include_directories(AsrCommmonHeaders INTERFACE ${CMAKE_SOURCE_DIR}/include/)
target_link_libraries(AsrUtils PUBLIC AsrCommmonHeaders)

add_library(ASR_EX_PRIVATE_LIBS ALIAS AsrUtils)

if(${BUILD_TEST})
    include(GoogleTest)
    asr_add_additional_test(AdditionalTest)
    target_link_libraries(AdditionalTest PUBLIC AsrUtils)
endif()

add_subdirectory(Core)

include(AsrSetLibPrefix)

add_subdirectory(Plugins)

if(${BUILD_QT5GUI})
    add_subdirectory(Qt5Gui)
endif()
