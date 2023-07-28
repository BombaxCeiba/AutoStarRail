#include "AutoStarRail/AsrPtr.hpp"
#include "AutoStarRail/AsrString.hpp"
#include "spdlog/common.h"
#include <AutoStarRail/ExportInterface/AsrLogger.h>
#include <AutoStarRail/Core/Logger/Logger.h>
#include <optional>

namespace AC = ASR::Core;

ASR_NS_ANONYMOUS_DETAILS_BEGIN

std::optional<spdlog::source_loc> ToSpdlogSourceLocation(
    const AsrSourceLocation* p_location)
{
    if (p_location == nullptr) [[unlikely]]
    {
        ASR_CORE_LOG_ERROR(
            "Received a null pointer of type AsrSourceLocation.");

        return std::nullopt;
    }

    spdlog::source_loc result;
    result.filename = p_location->file_name;
    result.line = p_location->line;
    result.funcname = p_location->function_name;

    return result;
}

ASR_NS_ANONYMOUS_DETAILS_END

void AsrLogError(IAsrReadOnlyString* p_string)
{
    Asr::AsrPtr<IAsrReadOnlyString> p_string_hodler{p_string};
    const char*                     p_u8_string{};

    p_string_hodler->GetUtf8(&p_u8_string);
    AsrLogErrorU8(p_u8_string);
}

void AsrLogErrorU8(const char* p_string) { AC::g_logger->info(p_string); }

void AsrLogErrorU8WithSourceLocation(
    const char*              p_string,
    const AsrSourceLocation* p_location)
{
    const auto opt_location = Details::ToSpdlogSourceLocation(p_location);
    if (opt_location)
    {
        AC::g_logger->log(opt_location.value(), spdlog::level::err, p_string);
    }
    else
    {
        AsrLogErrorU8(p_string);
    }
}

// ----------------------------------------------------------------

void AsrLogWarning(IAsrReadOnlyString* p_string)
{
    Asr::AsrPtr<IAsrReadOnlyString> p_string_hodler{p_string};
    const char*                     p_u8_string{};

    p_string_hodler->GetUtf8(&p_u8_string);
    AsrLogWarningU8(p_u8_string);
}

void AsrLogWarningU8(const char* p_string) { AC::g_logger->warn(p_string); }

void AsrLogWarningU8WithSourceLocation(
    const char*              p_string,
    const AsrSourceLocation* p_location)
{
    const auto opt_location = Details::ToSpdlogSourceLocation(p_location);
    if (opt_location)
    {
        AC::g_logger->log(opt_location.value(), spdlog::level::warn, p_string);
    }
    else
    {
        AsrLogErrorU8(p_string);
    }
}

// ----------------------------------------------------------------

void AsrLogInfo(IAsrReadOnlyString* p_string)
{
    Asr::AsrPtr<IAsrReadOnlyString> p_string_hodler{p_string};
    const char*                     p_u8_string{};

    p_string_hodler->GetUtf8(&p_u8_string);
    AsrLogInfoU8(p_u8_string);
}

void AsrLogInfoU8(const char* p_string) { AC::g_logger->info(p_string); }

void AsrLogInfoU8WithSourceLocation(
    const char*              p_string,
    const AsrSourceLocation* p_location)
{
    const auto opt_location = Details::ToSpdlogSourceLocation(p_location);
    if (opt_location)
    {
        AC::g_logger->log(opt_location.value(), spdlog::level::info, p_string);
    }
    else
    {
        AsrLogErrorU8(p_string);
    }
}

// ----------------------------------------------------------------

void AsrLogError(AsrString asr_string)
{
    Asr::AsrPtr<IAsrReadOnlyString> p_string{};
    asr_string.GetImpl(p_string.Put());
    AsrLogError(p_string.Get());
}

void AsrLogWarning(AsrString asr_string)
{
    Asr::AsrPtr<IAsrReadOnlyString> p_string{};
    asr_string.GetImpl(p_string.Put());
    AsrLogWarning(p_string.Get());
}

void AsrLogInfo(AsrString asr_string)
{
    Asr::AsrPtr<IAsrReadOnlyString> p_string{};
    asr_string.GetImpl(p_string.Put());
    AsrLogInfo(p_string.Get());
}