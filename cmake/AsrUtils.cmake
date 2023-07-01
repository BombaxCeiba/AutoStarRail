function(asr_add_library TYPE SUB_DIRECTORY_NAME PRIVATE_EX_LIBS)
    # file(GLOB_RECURSE HEADERS
    #     ${CMAKE_CURRENT_SOURCE_DIR}/${SUB_DIRECTORY_NAME}/include/*)
    file(GLOB SOURCES ${SUB_DIRECTORY_NAME}/src/*)
    add_library(${SUB_DIRECTORY_NAME} ${TYPE} ${SOURCES})
    target_include_directories(${SUB_DIRECTORY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/${SUB_DIRECTORY_NAME}/include)
    target_compile_options(${SUB_DIRECTORY_NAME} PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
    )
    if(${TYPE} STREQUAL "SHARED")
        target_compile_definitions(${SUB_DIRECTORY_NAME} PRIVATE -DASR_BUILD_SHARED)
    endif()

    target_link_libraries(${SUB_DIRECTORY_NAME} PUBLIC ${PRIVATE_EX_LIBS})
endfunction()

function(asr_add_core_conpoment SUB_DIRECTORY_NAME)
    # file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/${SUB_DIRECTORY_NAME}/include/*)
    file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/${SUB_DIRECTORY_NAME}/src/*)
    target_include_directories(AsrCoreObjects PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/${SUB_DIRECTORY_NAME}/include/)
    target_sources(AsrCoreObjects PRIVATE ${SOURCES})
endfunction()

macro(asr_force_set VARIABLE_NAME VARIABLE_VALUE VARIABLE_TYPE DOC_VALUE)
    set(${VARIABLE_NAME} ${VARIABLE_VALUE} CACHE ${VARIABLE_TYPE} ${DOC_VALUE} FORCE)
endmacro()

function(asr_add_additional_test ADDITIONAL_TEST_DIRECTORY_NAME)
    aux_source_directory(${ADDITIONAL_TEST_DIRECTORY_NAME} SOURCES)
    add_executable(AdditionalTest ${SOURCES})
    target_link_libraries(AdditionalTest PRIVATE GTest::gtest_main GTest::gtest)

    # force cmake output test executable to ${CMAKE_BINARY_DIR}/Test
    set_target_properties(AdditionalTest PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Test)
    set_target_properties(AdditionalTest PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Test)
    set_target_properties(AdditionalTest PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Test)
    set_target_properties(AdditionalTest PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/Test)
    set_target_properties(AdditionalTest PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_BINARY_DIR}/Test)

    gtest_discover_tests(
        AdditionalTest
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/Test)
endfunction()

function(asr_add_core_test TEST_FOLDER PRIVATE_EX_LIBS)
    aux_source_directory(${TEST_FOLDER}/test TEST_SOURCES)
    set(TEST_NAME "${TEST_FOLDER}Test")
    add_executable(${TEST_NAME} ${TEST_SOURCES})
    target_link_libraries(${TEST_NAME} PUBLIC GTest::gtest_main GTest::gtest AsrStaticCore ${PRIVATE_EX_LIBS})

    get_target_property(INCLUDE_DIRS AsrCoreObjects INCLUDE_DIRECTORIES)
    target_include_directories(${TEST_NAME} PRIVATE ${INCLUDE_DIRS})

    # force cmake output test executable to ${CMAKE_BINARY_DIR}/Test
    set_target_properties(${TEST_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Test)
    set_target_properties(${TEST_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Test)
    set_target_properties(${TEST_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Test)
    set_target_properties(${TEST_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/Test)
    set_target_properties(${TEST_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_BINARY_DIR}/Test)

    gtest_discover_tests(
        ${TEST_NAME}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/Test)
endfunction()

function(asr_add_swig_export_library LANGUAGE RAW_NAME FILES)
    swig_add_library(
        ${RAW_NAME}
        TYPE SHARED
        LANGUAGE ${LANGUAGE}
        OUTPUT_DIR ${CMAKE_BINARY_DIR}/include/AutoStarRail/${LANGUAGE}
        OUTFILE_DIR ${CMAKE_BINARY_DIR}/SwigCpp
        SOURCES ${FILES})
    set_property(TARGET ${RAW_NAME} PROPERTY SWIG_INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/include/)
    add_library(${PROJECT_NAME}::${LANGUAGE}Export ALIAS ${RAW_NAME})
endfunction()

function(asr_check_language_export LANGUAGE EXPORT_LANGUAGES_LIST)
    if(${EXPORT_${LANGUAGE}})
        string(TOLOWER ${LANGUAGE} LOWERCASE_LANGUAGE)
        list(APPEND ${EXPORT_LANGUAGES_LIST} ${LOWERCASE_LANGUAGE})
        set(${EXPORT_LANGUAGES_LIST} ${${EXPORT_LANGUAGES_LIST}} PARENT_SCOPE)
    endif()
endfunction()
