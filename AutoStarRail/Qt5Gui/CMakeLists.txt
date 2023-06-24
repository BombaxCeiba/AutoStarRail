include(AsrSetLibPrefixAsDefaultValue)
include(AsrQtUtils)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(QRC_SHARED_LIBS "")
file(GLOB QRC_SOURCES Resources/*qrc)
foreach(QRC_FILE_PATH ${QRC_SOURCES})
    asr_add_qrc_shared_lib_from_file(${QRC_FILE_PATH} QRC_SHARED_LIBS)
endforeach()

file(GLOB_RECURSE SOURCES Qt5Gui/src/*)
add_executable(AutoStarRailQt5Gui ${SOURCES} ${QRC_SOURCES})
target_link_libraries(AutoStarRailQt5Gui PRIVATE
    libAsrCore
    ASR_EX_PRIVATE_LIBS
    ${QRC_SHARED_LIBS}
    Qt::Widgets
    Qt::Qml
    Qt::Quick)

if(WIN32)
    target_compile_definitions(AutoStarRailQt5Gui PRIVATE ASR_WIN32)
endif()