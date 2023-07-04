include(FetchContent)
include(ExternalProject)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

# config SWIG
include(UseSWIG)

if(POLICY CMP0122)
    cmake_policy(SET CMP0122 NEW)
endif()

set(ASR_EXPORT_LANGUAGES_LIST "")
set(SWIG_COMPONENTS "")

asr_check_language_export(JAVA ASR_EXPORT_LANGUAGES_LIST)
asr_check_language_export(CSHARP ASR_EXPORT_LANGUAGES_LIST)
asr_check_language_export(PYTHON ASR_EXPORT_LANGUAGES_LIST)

if(NOT ASR_EXPORT_LANGUAGES_LIST)
    message(STATUS "No interface needs to be exported.")
    message(STATUS "Skipping search SWIG executable.")
else()
    message(STATUS "The project will be exported as an interface to the following languages:\n${ASR_EXPORT_LANGUAGES_LIST}")

    # list(REMOVE_ITEM ASR_EXPORT_LANGUAGES_LIST lua)
    find_package(SWIG 4.1.1 COMPONENTS ${ASR_EXPORT_LANGUAGES_LIST} REQUIRED)

    if(SWIG_FOUND)
        message(STATUS "SWIG found: ${SWIG_EXECUTABLE}")
        set(SWIG_SOURCE_FILE_EXTENSIONS ".i" ".swg")
    endif()
endif()

add_custom_target(AsrAutoCopyDll ALL DEPENDS AsrCoreObjects)

set(CMAKE_CXX_STANDARD 20)
add_library(Asr3rdParty INTERFACE)

# config GoogleTest
# GoogleTest requires at least C++14
message(STATUS "Downloading and configuring google test.")
FetchContent_Declare(
    googletest
    URL ${GITHUB_MIRROR_URL}/google/googletest/archive/refs/tags/v1.13.0.tar.gz
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

# config QT5
if(${BUILD_QT5_GUI})
    message(STATUS "Option BUILD_QT5_GUI=ON. Qt5 is required.")
    find_package(Qt5 COMPONENTS Widgets Qml Quick REQUIRED)
endif()

# config magic_enum
message(STATUS "Downloading and configuring magic enum.")
FetchContent_Declare(
    MagicEnum
    URL "${GITHUB_MIRROR_URL}/Neargye/magic_enum/archive/refs/tags/v0.8.2.zip"
)

FetchContent_MakeAvailable(MagicEnum)
target_link_libraries(Asr3rdParty INTERFACE magic_enum::magic_enum)

if(ASR_USE_BUNDLIED_FMT)
    message(STATUS "Downloading and configuring fmt.")
    FetchContent_Declare(
        Asrfmt
        URL "${GITHUB_MIRROR_URL}/fmtlib/fmt/releases/download/10.0.0/fmt-10.0.0.zip")
    FetchContent_MakeAvailable(Asrfmt)
    set_target_properties(fmt PROPERTIES POSITION_INDEPENDENT_CODE ON)
    target_link_libraries(Asr3rdParty INTERFACE fmt::fmt)
    target_compile_definitions(Asr3rdParty INTERFACE -DASR_USE_STD_FMT=0)
else()
    target_compile_definitions(Asr3rdParty INTERFACE -DASR_USE_STD_FMT=1)
endif()

# config spdlog
message(STATUS "Downloading and configuring spdlog.")
FetchContent_Declare(
    spdlog
    URL "${GITHUB_MIRROR_URL}/gabime/spdlog/archive/refs/tags/v1.11.0.zip")

set(SPDLOG_BUILD_PIC ON)

if(ASR_SPDLOG_USE_STD_FMT)
    set(SPDLOG_USE_STD_FORMAT ON)
endif()

if(WIN32)
    set(SPDLOG_WCHAR_SUPPORT ON)
    set(SDLOG_WCHAR_FILENAMES ON)
endif()

FetchContent_MakeAvailable(spdlog)
target_link_libraries(Asr3rdParty INTERFACE spdlog::spdlog)

function(_download_and_config_opencv)
    message(STATUS "Downloading OpenCV source.")

    # NOTICE: OpenCV wil turn this option ON automically, so I explicitly set it.
    set(BUILD_SHARED_LIBS ON)

    include(AsrResetLibPrefix)

    FetchContent_Declare(
        OpenCV48
        URL ${GITHUB_MIRROR_URL}/opencv/opencv/archive/refs/tags/4.8.0.tar.gz
    )

    if(${ASR_USE_CCACHE})
        set(ENABLE_CCACHE ON)
    endif()

    # config OpenCV
    set(BUILD_opencv_apps OFF)
    set(BUILD_opencv_calib3d OFF)
    set(BUILD_opencv_dnn OFF)
    set(BUILD_opencv_features2d OFF)
    set(BUILD_opencv_highgui OFF)
    set(BUILD_opencv_gapi OFF)
    set(BUILD_opencv_flann OFF)
    set(BUILD_opencv_ml OFF)
    set(BUILD_opencv_photo OFF)
    set(BUILD_opencv_stitching OFF)
    set(BUILD_opencv_video OFF)
    set(BUILD_opencv_videoio OFF)
    set(BUILD_opencv_world OFF)
    set(WITH_OPENCL OFF)
    set(BUILD_opencv_python_bindings_generator OFF)
    set(BUILD_opencv_python_test OFF)
    set(BUILD_opencv_java_bindings_generator OFF)
    set(BUILD_opencv_java_bindings_generator OFF)
    set(BUILD_opencv_js_bindings_generator OFF)
    set(BUILD_opencv_js OFF)
    set(BUILD_opencv_ts OFF)

    FetchContent_MakeAvailable(OpenCV48)
    list(APPEND ASR_OPENCV_COMPONENTS opencv_core opencv_imgproc)
    set_target_properties(${ASR_OPENCV_COMPONENTS} PROPERTIES PREFIX ${ASR_LIB_PREFIX})
    target_link_libraries(Asr3rdParty INTERFACE ${ASR_OPENCV_COMPONENTS})
endfunction()

if(ASR_USE_BUNDLIED_OPENCV)
    if(ASR_USE_BUNDLIED_OPENCV)
        if(${ASR_USE_BUNDLIED_EIGEN})
            message(STATUS "Downloading Eigen3.4...")
            FetchContent_Declare(AsrEigen3
                URL https://gitlab.com/libeigen/eigen/-/archive/3.4.0/eigen-3.4.0.zip)
            FetchContent_MakeAvailable(AsrEigen3)
            set(WITH_EIGEN ON)
        else()
            find_package(Eigen3 3.4 NO_MODULE)

            if(NOT Eigen3_FOUND)
                set(WITH_EIGEN ON)
            else()
                set(WITH_EIGEN OFF)
            endif()
        endif()

        _download_and_config_opencv()
    else()
        find_package(OpenCV 4.8)

        if(NOT OpenCV_FOUND)
            target_link_libraries(Asr3rdParty INTERFACE ${OpenCV_LIBS})
        else()
            message(FATAL_ERROR "OpenCV not found. Please install it manually or set ASR_USE_BUNDLIED_OPENCV=ON.")
        endif()
    endif()
    _download_and_config_opencv()
else()
    find_package(OpenCV 4.8)

    if(NOT OpenCV_FOUND)
        target_link_libraries(Asr3rdParty INTERFACE ${OpenCV_LIBS})
    else()
        message(FATAL_ERROR "OpenCV not found. Please install it manually or set ASR_USE_BUNDLIED_OPENCV=ON.")
    endif()
endif()

function(_on_download_icu)
    message("Downloading prebuilt icu binary...")
endfunction()

if(WIN32 AND CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(ASR_ICU_VERSION 73)

    if(${PROCESSOR_ARCHITECTURE} MATCHES "ARM64")
        # arm64
        _on_download_icu()
        FetchContent_Declare(
            icu_msvc_2019_binary_arm64
            URL ${GITHUB_MIRROR_URL}/unicode-org/icu/releases/download/release-73-1/icu4c-73_1-WinARM64-MSVC2019.zip
            CONFIGURE_COMMAND ""
            BUILD_COMMAND ""
            INSTALL_COMMAND "")
        FetchContent_MakeAvailable(icu_msvc_2019_binary_arm64)
        set(ICU_ROOT ${icu_msvc_2019_binary_arm64_SOURCE_DIR})
        set(ICU_DIR ${icu_msvc_2019_binary_arm64_SOURCE_DIR})
        list(APPEND ASR_BUNDLIED_ICU_LIBRARIES
            ${icu_msvc_2019_binary_arm64_SOURCE_DIR}/binARM64/icudt${ASR_ICU_VERSION}.dll
            ${icu_msvc_2019_binary_arm64_SOURCE_DIR}/binARM64/icuin${ASR_ICU_VERSION}.dll
            ${icu_msvc_2019_binary_arm64_SOURCE_DIR}/binARM64/icuuc${ASR_ICU_VERSION}.dll)
    else()
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            # 64 bits
            _on_download_icu()
            FetchContent_Declare(
                icu_msvc_2019_binary_win64
                URL ${GITHUB_MIRROR_URL}/unicode-org/icu/releases/download/release-73-1/icu4c-73_1-Win64-MSVC2019.zip
                CONFIGURE_COMMAND ""
                BUILD_COMMAND ""
                INSTALL_COMMAND "")
            FetchContent_MakeAvailable(icu_msvc_2019_binary_win64)
            set(ICU_ROOT ${icu_msvc_2019_binary_win64_SOURCE_DIR})
            set(ICU_DIR ${icu_msvc_2019_binary_win64_SOURCE_DIR})
            list(APPEND ASR_BUNDLIED_ICU_LIBRARIES
                ${icu_msvc_2019_binary_win64_SOURCE_DIR}/bin64/icudt${ASR_ICU_VERSION}.dll
                ${icu_msvc_2019_binary_win64_SOURCE_DIR}/bin64/icuin${ASR_ICU_VERSION}.dll
                ${icu_msvc_2019_binary_win64_SOURCE_DIR}/bin64/icuuc${ASR_ICU_VERSION}.dll)
        elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
            # 32 bits
            _on_download_icu()
            FetchContent_Declare(
                icu_msvc_2019_binary_win32
                URL ${GITHUB_MIRROR_URL}/unicode-org/icu/releases/download/release-73-1/icu4c-73_1-Win32-MSVC2019.zip
                CONFIGURE_COMMAND ""
                BUILD_COMMAND ""
                INSTALL_COMMAND "")
            FetchContent_MakeAvailable(icu_msvc_2019_binary_win32)
            set(ICU_ROOT ${icu_msvc_2019_binary_win32_SOURCE_DIR})
            set(ICU_DIR ${icu_msvc_2019_binary_win32_SOURCE_DIR})
            list(APPEND ASR_BUNDLIED_ICU_LIBRARIES
                ${icu_msvc_2019_binary_win32_SOURCE_DIR}/bin/icudt${ASR_ICU_VERSION}.dll
                ${icu_msvc_2019_binary_win32_SOURCE_DIR}/bin/icuin${ASR_ICU_VERSION}.dll
                ${icu_msvc_2019_binary_win32_SOURCE_DIR}/bin/icuuc${ASR_ICU_VERSION}.dll)
        endif()
    endif()

    find_package(ICU COMPONENTS data i18n uc REQUIRED)

    add_custom_command(
        TARGET AsrAutoCopyDll
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ASR_BUNDLIED_ICU_LIBRARIES} $<TARGET_FILE_DIR:libAsrCore>)
    add_custom_command(
        TARGET AsrAutoCopyDll
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ASR_BUNDLIED_ICU_LIBRARIES} ${CMAKE_BINARY_DIR}/Test)
endif()

find_package(ICU COMPONENTS data i18n uc REQUIRED)

function(_download_and_config_boost)
    message("Downloading and configuring boost 1.82.")

    # set(BUILD_SHARED_LIBS ON)
    set(BUILD_TESTING OFF)

    FetchContent_Declare(
        AsrBoost
        URL ${GITHUB_MIRROR_URL}/boostorg/boost/releases/download/boost-1.82.0/boost-1.82.0.7z
        PATCH_COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_SOURCE_DIR}/cmake/AsrBoostContext182Patch.cmake" <SOURCE_DIR>/libs/context/CMakeLists.txt)

    # See [Symbol redefinition on latest mingw gcc](https://github.com/boostorg/stacktrace/issues/133)
    # You should use boost from msys package manager to avoid this error.
    # set(BOOST_STACKTRACE_ENABLE_WINDBG_CACHED OFF)
    # set(BOOST_STACKTRACE_ENABLE_WINDBG OFF)

    # Avoid error: "LINK : fatal error LNK1104: 无法打开文件“zlib.obj”
    set(BOOST_IOSTREAMS_ENABLE_ZLIB OFF)

    # Fix error: .../libs/asio/include/boost/asio/detail/impl/winsock_init.ipp:39: undefined reference to `__imp_WSAStartup'
    if(WIN32)
        link_libraries(ws2_32)
    endif()

    FetchContent_MakeAvailable(AsrBoost)

    set_target_properties(boost_filesystem PROPERTIES POSITION_INDEPENDENT_CODE ON)
endfunction()

if(${ASR_USE_BUNDLIED_BOOST})
    _download_and_config_boost()
    set(ASR_USE_BUNDLIED_BOOST TRUE)
    list(APPEND ASR_BUNDLIED_BOOST_LIBS Boost::regex_icu Boost::dll Boost::predef)
else()
    find_package(Boost 1.82 COMPONENTS filesystem)

    if(NOT Boost_FOUND)
        message(FATAL_ERROR "Boost not found. Please install it manually or set ASR_USE_BUNDLIED_BOOST=ON.")
    else()
        set(ASR_USE_BUNDLIED_BOOST OFF)
    endif()
endif()

# config Nlohnmann Json
# Notice: This library assume string is encoded in UTF-8.
message("Downloading and configuring nlohmann json.")
FetchContent_Declare(
    NlohnmannJson
    URL ${GITHUB_MIRROR_URL}/nlohmann/json/archive/refs/tags/v3.11.2.tar.gz)

FetchContent_MakeAvailable(NlohnmannJson)

target_link_libraries(Asr3rdParty INTERFACE nlohmann_json::nlohmann_json)

# See [added support for cmake "find_package"](https://github.com/microsoft/onnxruntime/pull/8919)
find_package(onnxruntime)

function(_download_and_config_onnxruntime TARGET_NAME URL)
    FetchContent_Declare(${TARGET_NAME}
        URL ${URL}
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND "")
    FetchContent_MakeAvailable(${TARGET_NAME})
    FetchContent_GetProperties(${TARGET_NAME})
    add_library(AsrBundledOnnxRuntime SHARED IMPORTED)
    find_library(${TARGET_NAME}_DLL onnxruntime "${${TARGET_NAME}_SOURCE_DIR}/lib")
    set_target_properties(
        AsrBundledOnnxRuntime
        PROPERTIES
        IMPORTED_CONFIGURATIONS RELEASE
        IMPORTED_LOCATION "${${TARGET_NAME}_DLL}"
        INTERFACE_INCLUDE_DIRECTORIES "${${TARGET_NAME}_SOURCE_DIR}/include"
        INTERFACE_COMPILE_OPTIONS "")
    set_property(
        TARGET AsrBundledOnnxRuntime
        APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)

    if(WIN32)
        set_property(
            TARGET AsrBundledOnnxRuntime
            PROPERTY IMPORTED_IMPLIB "${${TARGET_NAME}_SOURCE_DIR}/lib/onnxruntime.lib")
    endif()
endfunction()

if(NOT onnxruntime_FOUND)
    message("Downloading prebuilt onnxruntime binary...")
    if(WIN32)
        if(${PROCESSOR_ARCHITECTURE} MATCHES "ARM64")
            # arm64
            _download_and_config_onnxruntime(
                "onnxruntime_msvc_amd64"
                "${GITHUB_MIRROR_URL}/microsoft/onnxruntime/releases/download/v1.15.1/onnxruntime-win-arm64-1.15.1.zip")
        elseif(${PROCESSOR_ARCHITECTURE} MATCHES "ARM")
            # arm
            _download_and_config_onnxruntime(
                "onnxruntime_msvc_amd64"
                "${GITHUB_MIRROR_URL}/microsoft/onnxruntime/releases/download/v1.15.1/onnxruntime-win-arm-1.15.1.zip")
        elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
            # amd64
            _download_and_config_onnxruntime(
                "onnxruntime_msvc_amd64"
                "${GITHUB_MIRROR_URL}/microsoft/onnxruntime/releases/download/v1.15.1/onnxruntime-win-x64-1.15.1.zip")
        elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
            # x86
            _download_and_config_onnxruntime(
                "onnxruntime_msvc_x86"
                "${GITHUB_MIRROR_URL}/microsoft/onnxruntime/releases/download/v1.15.1/onnxruntime-win-x86-1.15.1.zip")
        else()
            message(FATAL_ERROR "Unsupported architecture.")
        endif()
    elseif(APPLE)
        # TODO
        message(FATAL_ERROR "Not yet supported. PR welcome.")
    elseif(UNIX)
        if(${PROCESSOR_ARCHITECTURE} MATCHES "ARM")
            # arm64
            _download_and_config_onnxruntime(
                "onnxruntime_linux_arm64"
                "${GITHUB_MIRROR_URL}/microsoft/onnxruntime/releases/download/v1.15.1/onnxruntime-linux-aarch64-1.15.1.tgz")
        elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
            # amd64
            _download_and_config_onnxruntime(
                "onnxruntime_linux_amd64"
                "${GITHUB_MIRROR_URL}/microsoft/onnxruntime/releases/download/v1.15.1/onnxruntime-linux-x64-1.15.1.tgz")
        elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
            # x86
            message(FATAL_ERROR "No official prebuilt binary for this architecture.")
        else()
            message(FATAL_ERROR "Unsupported architecture.")
        endif()
    endif()

    target_link_libraries(Asr3rdParty INTERFACE AsrBundledOnnxRuntime)
else()
    target_link_libraries(Asr3rdParty INTERFACE onnxruntime::onnxruntime)
endif()
