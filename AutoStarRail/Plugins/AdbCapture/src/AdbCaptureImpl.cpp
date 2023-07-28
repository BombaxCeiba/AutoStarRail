#include <AutoStarRail/AsrConfig.h>

ASR_DISABLE_WARNING_BEGIN
ASR_IGNORE_UNUSED_PARAMETER

#include <boost/process/async.hpp>

ASR_DISABLE_WARNING_END

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4068)
#endif
#include <gzip/decompress.hpp>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include "AdbCaptureImpl.h"
#include <AutoStarRail/AsrConfig.h>
#include <AutoStarRail/ExportInterface/AsrLogger.h>
#include <AutoStarRail/Utils/QueryInterfaceImpl.hpp>
#include <AutoStarRail/Utils/Utils.hpp>
#include <AutoStarRail/Utils/IAsrBaseAdapterUtils.h>
#include <AutoStarRail/Utils/fmt.h>
#include <boost/process/async_pipe.hpp>
#include <boost/process/detail/child_decl.hpp>
#include <boost/process/pipe.hpp>
#include <boost/process.hpp>
#include <boost/asio.hpp>
#include <array>
#include <system_error>
#include <tuple>
#include <sstream>

ASR_NS_BEGIN

// reference from
// https://developer.android.com/reference/android/graphics/PixelFormat
// https://android.googlesource.com/platform/frameworks/base/+/android-4.3_r2.3/cmds/screencap/screencap.cpp
// https://android.googlesource.com/platform/frameworks/base/+/refs/heads/android-s-beta-4/cmds/screencap/screencap.cpp
// NOTE: kN32_SkColorType selects the native 32-bit ARGB format. On little
// endian processors, pixels containing 8-bit ARGB components pack into 32-bit
//  kBGRA_8888_SkColorType. On big endian processors, pixels pack into 32-bit
//  kRGBA_8888_SkColorType.
// In this plugin, we assume the color type is RGBA_8888.
enum class AdbCaptureFormat
{
    JPEG = 256,
    RGBA_8888 = 1,
    RGBX_8888 = 2,
    RGB_888 = 3,
    RGB_565 = 4
};

AdbCapture::AdbCapture(
    const std::filesystem::path& adb_path,
    std::string_view             adb_device_serial)
    : capture_png_command_{ASR::fmt::format(
        "{} -s {} exec-out screencap -p",
        adb_path.string(),
        adb_device_serial)},
      capture_gzip_raw_command_{ASR::fmt::format(
          R"({} -s {} exec-out "screencap | gzip -1")",
          adb_path.string(),
          adb_device_serial)},
      get_screen_size_command_{ASR::fmt::format(
          R"({} -s {} shell dumpsys window displays | grep -o -E cur=+[^\\ ]+ | grep -o -E [0-9]+)",
          adb_path.string(),
          adb_device_serial)}
//   get_nc_address_command_{ASR::fmt::format(
//       R"({} -s {} shell " cat /proc/net/arp | grep : ")",
//       adb_path.string(),
//       adb_device_serial)}
//   capture_raw_by_nc_command_{ASR::fmt::format(
//       R"({} -s {} exec-out "screencap | nc -w 3 {} {}")",
//       adb_path.string(),
//       adb_device_serial,
//       nc_address_,
//       nc_port_)},
{
}

ASR_NS_ANONYMOUS_DETAILS_BEGIN

std::size_t ComputeScreenshotSize(
    const std::int32_t width,
    const std::int32_t height) noexcept
{
    // header + data (assume 32bit color)
    return 16 + width * height * 4;
}

template <class Buffer>
struct CommandExecutorContext : public ASR::Utils::NonCopyableAndNonMovable
{
private:
    void OnProcessExited(int exit_code, const std::error_code& error_code)
    {
        timer.cancel();

        if (exit_code || error_code)
        {
            constexpr const auto& string_template =
                R"(Executing command: "{}".\nExit code: {}.\nError code: {}. Message: "{}".)";
            const auto error_code_message = error_code.message();
            const auto error_message = ASR::fmt::format(
                string_template,
                this->command,
                exit_code,
                error_code.value(),
                error_code_message.c_str());
            AsrLogErrorU8(error_message.c_str());
        }
        else
        {
            constexpr const auto& string_template =
                R"(Executing command: "{}" successfully.)";
            const auto error_message =
                ASR::fmt::format(string_template, this->command);
            AsrLogInfoU8(error_message.c_str());
        }
    }

    boost::asio::awaitable<void> WaitProcessTimeout()
    {
        while (true)
        {
            const auto [error_code] = co_await timer.async_wait(
                boost::asio::as_tuple(boost::asio::use_awaitable));
            if (error_code != boost::asio::error::operation_aborted) [[likely]]
            {
                break;
            }
            else if (error_code == boost::system::errc::success)
            {
                process.terminate();
                process_out.close();
            }
            else
            {
                const auto error_message = ASR::fmt::format(
                    R"(Unexpected error when waiting timeout.\n Error code: {}. Message: "{}".)",
                    error_code.value(),
                    error_code.message());
                AsrLogErrorU8(error_message.c_str());
            }
        }
    }

public:
    template <class T>
    CommandExecutorContext(
        std::string_view    command,
        const std::uint32_t timeout,
        T&&                 buffer)
        requires(!std::is_lvalue_reference_v<T>)
        : buffer{std::forward<T>(buffer)},
          process{
              command.data(),
              boost::process::std_out > process_out,
              ioc,
              boost::process::on_exit(
                  [this](int exit_code, const std::error_code& error_code)
                  { this->OnProcessExited(exit_code, error_code); })},
          timer{ioc, std::chrono::seconds(timeout)}, timeout{timeout},
          command{command}
    {
        // 超时
        boost::asio::co_spawn(
            ioc,
            [this]() { return this->WaitProcessTimeout(); }(),
            boost::asio::detached);
        // 读取输出
        boost::asio::co_spawn(
            ioc,
            [this]() -> boost::asio::awaitable<void>
            {
                const auto [error_code, read_length] =
                    co_await boost::asio::async_read(
                        process_out,
                        boost::asio::dynamic_buffer(this->buffer),
                        boost::asio::as_tuple(boost::asio::use_awaitable));
                if (!error_code) [[unlikely]]
                {
                    const auto error_message = ASR::fmt::format(
                        "Unexpected error when reading stdout for command: {}.\n Error code: {}. Message: {}",
                        this->command.data(),
                        error_code.value(),
                        error_code.message());
                    AsrLogErrorU8(error_message.c_str());
                }
            }(),
            [](std::exception_ptr p_ex)
            {
                try
                {
                    if (p_ex)
                    {
                        std::rethrow_exception(p_ex);
                    }
                }
                catch (const std::runtime_error& ex)
                {
                    AsrLogErrorU8(ex.what());
                }
            });
    }
    Buffer                     buffer;
    boost::asio::io_context    ioc{};
    boost::process::async_pipe process_out{ioc};
    boost::process::child      process;
    boost::asio::steady_timer  timer;
    std::uint32_t              timeout;
    std::string                command;
};

ASR_NS_ANONYMOUS_DETAILS_END

template <class T>
auto MakeCommandExecutorContext(
    std::string_view    command,
    const std::uint32_t timeout)
{
    T tmp_buffer{};
    return Details::CommandExecutorContext<T>{
        command,
        timeout,
        std::move(tmp_buffer)};
}

ASR::Utils::Expected<AdbCapture::Size> AdbCapture::GetDeviceSize() const
{
    auto context =
        MakeCommandExecutorContext<std::string>(get_screen_size_command_, 10);
    Size result{};
    context.ioc.run();
    std::stringstream output_string_stream{context.buffer};
    int               size1{0};
    int               size_2{0};
    output_string_stream >> size1 >> size_2;

    result.width = (std::max)(size1, size_2);
    result.height = (std::min)(size1, size_2);
    return result;
}

void AdbCapture::CaptureRawWithGZip()
{
    // Initialize buffer.
    auto adb_output_buffer = ASR::Utils::MakeContainerOfSize<std::vector<char>>(
        Details::ComputeScreenshotSize(
            adb_device_screen_size_.width,
            adb_device_screen_size_.height));
    // Run adb and receive screen capture.
    Details::CommandExecutorContext<decltype(adb_output_buffer)> context{
        capture_gzip_raw_command_,
        10,
        std::move(adb_output_buffer)};
    // Initialize the objects that need to be used later.
    auto decompressed_data = ASR::Utils::MakeContainerOfSize<std::vector<char>>(
        Details::ComputeScreenshotSize(
            adb_device_screen_size_.width,
            adb_device_screen_size_.height));
    gzip::Decompressor decompressor{};
    // wait for the process to exit.
    context.ioc.run();

    decompressor.decompress(
        decompressed_data,
        context.buffer.data(),
        context.buffer.size());
}

void AdbCapture::CaptureRaw() {}

void AdbCapture::CapturePng() {}

void AdbCapture::CaptureRawByNc() {}

void AdbCapture::AutoDetectType() {}

int64_t AdbCapture::AddRef() { return ref_counter_.AddRef(); }

int64_t AdbCapture::Release() { return ref_counter_.Release(this); }

AsrResult AdbCapture::QueryInterface(const AsrGuid& iid, void** pp_object)
{
    return ASR::Utils::QueryInterface<IAsrCapture>(this, iid, pp_object);
}

AsrResult AdbCapture::Capture(IAsrImage** pp_out_image)
{
    (void)pp_out_image;
    return ASR_E_NO_IMPLEMENTATION;
}

ASR_NS_END