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

set(CMAKE_CXX_STANDARD 20)
add_library(Asr3rdParty INTERFACE)

# config GoogleTest
# GoogleTest requires at least C++14
FetchContent_Declare(
    googletest
    GIT_REPOSITORY ${GITHUB_MIRROR_URL}/google/googletest.git
    GIT_TAG v1.13.0
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

# # config dylib
# FetchContent_Declare(
# dylib
# URL "${GITHUB_MIRROR_URL}/martin-olivier/dylib/archive/refs/tags/v2.1.0.zip"
# )

# FetchContent_MakeAvailable(dylib)

# config QT5
if(${BUILD_QT5_GUI})
    find_package(Qt5 COMPONENTS Widgets Qml Quick REQUIRED)
endif()

# config magic_enum
FetchContent_Declare(
    MagicEnum
    URL "${GITHUB_MIRROR_URL}/Neargye/magic_enum/archive/refs/tags/v0.8.2.zip"
)

FetchContent_MakeAvailable(MagicEnum)
target_link_libraries(Asr3rdParty INTERFACE magic_enum::magic_enum)

# config spdlog
FetchContent_Declare(
    spdlog
    URL "${GITHUB_MIRROR_URL}/gabime/spdlog/archive/refs/tags/v1.11.0.zip"
)

if(ASR_SPDLOG_USE_STD_FMT)
    set(SPDLOG_USE_STD_FORMAT ON)
endif()

if(WIN32)
    set(SPDLOG_WCHAR_SUPPORT ON)
    set(SDLOG_WCHAR_FILENAMES ON)
endif()

FetchContent_MakeAvailable(spdlog)
target_link_libraries(Asr3rdParty INTERFACE spdlog::spdlog)

# NOTICE: OpenCV wil turn this option ON automically, so I explicitly set it.
set(BUILD_SHARED_LIBS ON)

find_package(OpenCV 4.7)

if(NOT OpenCV_FOUND)
    message("OpenCV not found. Downloading OpenCV source.")

    FetchContent_Declare(
        OpenCV47
        URL ${GITHUB_MIRROR_URL}/opencv/opencv/archive/refs/tags/4.7.0.zip
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
    set(WITH_EIGEN OFF)
    set(WITH_OPENCL OFF)
    set(BUILD_opencv_python_bindings_generator OFF)
    set(BUILD_opencv_python_test OFF)
    set(BUILD_opencv_java_bindings_generator OFF)
    set(BUILD_opencv_java_bindings_generator OFF)
    set(BUILD_opencv_js_bindings_generator OFF)
    set(BUILD_opencv_js OFF)
    set(BUILD_opencv_ts OFF)

    FetchContent_MakeAvailable(OpenCV47)
    target_link_libraries(Asr3rdParty INTERFACE opencv_core opencv_imgproc)
else()
    target_link_libraries(Asr3rdParty INTERFACE ${OpenCV_LIBS})
endif()

function(_on_download_icu)
    message("Downloading prebuilt icu binary.")
    set(ASR_USE_BUNDLIED_ICU TRUE PARENT_SCOPE)
endfunction()

# TODO: Add ICU PDB files to build directory.
# TODO: Need to be tested on MSVC environment
set(ASR_USE_BUNDLIED_ICU FALSE)

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
endif()

find_package(ICU COMPONENTS data i18n uc REQUIRED)

find_package(Boost 1.82 COMPONENTS filesystem)

function(_download_and_config_boost)
    message("Downloading and confing boost 1.82.")
    FetchContent_Declare(
        AsrBoost
        URL ${GITHUB_MIRROR_URL}/boostorg/boost/releases/download/boost-1.82.0/boost-1.82.0.7z)

    # See [Symbol redefinition on latest mingw gcc](https://github.com/boostorg/stacktrace/issues/133)
    set(BOOST_STACKTRACE_ENABLE_WINDBG_CACHED OFF)
    set(BOOST_STACKTRACE_ENABLE_WINDBG OFF)

    # Fix error: .../libs/asio/include/boost/asio/detail/impl/winsock_init.ipp:39: undefined reference to `__imp_WSAStartup'
    if(WIN32)
        link_libraries(ws2_32)
    endif()

    FetchContent_MakeAvailable(AsrBoost)
endfunction()

if(NOT Boost_FOUND)
    _download_and_config_boost()
endif()

# config Nlohnmann Json
# Notice: This library assume string is encoded in UTF-8.
FetchContent_Declare(
    NlohnmannJson
    GIT_REPOSITORY ${GITHUB_MIRROR_URL}/nlohmann/json.git
    GIT_TAG v3.11.2
)

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
