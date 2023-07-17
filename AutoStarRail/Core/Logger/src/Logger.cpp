#include <AutoStarRail/Core/Logger/Logger.h>
#include <array>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>

#if defined(_WIN32) || defined(__CYGWIN__)
#include <windows.h>
ASR_NS_ANONYMOUS_DETAILS_BEGIN
void UseUtf8Console()
{
    ::SetConsoleOutputCP(CP_UTF8);
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof cfi;
    cfi.nFont = 0;
    cfi.dwFontSize.X = 0;
    cfi.dwFontSize.Y = 14;
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;
    ::wcscpy_s(cfi.FaceName, LF_FACESIZE, L"Lucida Console");
    if (::SetCurrentConsoleFontEx(
            ::GetStdHandle(STD_OUTPUT_HANDLE),
            FALSE,
            &cfi)
        == 0)
    {
        const auto error_code = ::GetLastError();
        SPDLOG_ERROR("Failed to set console font. GetLastError={}", error_code);
    }
}
ASR_NS_ANONYMOUS_DETAILS_END
#define ASR_FORCE_CONSOLE_UTF8 Details::UseUtf8Console()
#else
#define ASR_FORCE_CONSOLE_UTF8
#endif // ASR_WIN32

ASR_NS_BEGIN

namespace Core
{
    const std::shared_ptr<spdlog::logger> g_logger = []()
    {
        const auto std_sink = std::make_shared<spdlog::sinks::stdout_sink_mt>();
        const auto file_sink =
            std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                "logs/libAsrCore.log",
                50 * 1024 * 1024, // 50mb
                2);
        const auto sinks = std::array<spdlog::sink_ptr, 2>{std_sink, file_sink};
        const auto result = std::make_shared<spdlog::logger>(
            g_logger_name,
            std::begin(sinks),
            std::end(sinks));
        spdlog::register_logger(result);
        spdlog::set_pattern(
            "[%Y-%m-%d %H:%M:%S.%e][thr %t][%l][%s:%!():%#][%i] %v");

        spdlog::set_level(spdlog::level::trace);

        ASR_FORCE_CONSOLE_UTF8;

        SPDLOG_LOGGER_INFO(result, "The logger has been initialized.");

        return result;
    }();

    const char* const g_logger_name = "asr_core_g_logger";

    TraceScope::TraceScope(
        const char* const file,
        int               line,
        const char* const func)
        : file_{file}, line_{line}, func_{func}
    {
        g_logger->log(
            spdlog::source_loc{file_, line_, func_},
            spdlog::level::trace,
            "Scope in.");
    }

    TraceScope::~TraceScope()
    {
        g_logger->log(
            spdlog::source_loc{file_, line_, func_},
            spdlog::level::trace,
            "Scope out.");
    }
}

ASR_NS_END