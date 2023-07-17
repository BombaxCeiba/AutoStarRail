#ifndef ASR_UTILS_EXPECTED_H
#define ASR_UTILS_EXPECTED_H

#include <AutoStarRail/Utils/Config.h>
#include <AutoStarRail/AsrString.hpp>
#include <AutoStarRail/Utils/fmt.h>
#include <tl/expected.hpp>
#include <variant>
#include <type_traits>

#define ASTR_UTILS_LOG_ON_ERROR                                                \
    [](const auto& unexpected) { ASR_CORE_LOG_ERROR(unexpected.explanation); }

ASR_UTILS_NS_BEGIN

using VariantString =
    std::variant<AsrString, std::string, const char*, std::string_view>;

namespace Details
{
    extern const char* const NULL_STRING;
}

struct ErrorAndExplanation
{
    explicit ErrorAndExplanation(const AsrResult error_code) noexcept;
    template <class T>
    explicit ErrorAndExplanation(const AsrResult error_code, T&& explanation)
        : error_code{error_code}, explanation{std::forward<T>(explanation)}
    {
    }

    AsrResult     error_code;
    VariantString explanation{Details::NULL_STRING};
};

template <class... Args>
auto MakeUnexpected(Args&&... args) -> tl::unexpected<ErrorAndExplanation>
{
    return tl::unexpected<ErrorAndExplanation>{std::forward<Args>(args)...};
}

template <class T>
using ExpectedWithExplanation = tl::expected<T, ErrorAndExplanation>;

template <class T>
using Expected = tl::expected<T, AsrResult>;

template <class T, class OnOk>
AsrResult Match(const ::tl::expected<T, AsrResult>& expected, OnOk&& on_ok)
{
    if (expected)
    {
        if constexpr (std::is_same_v<
                          std::
                              invoke_result_t<OnOk, decltype(expected.value())>,
                          void>)
        {
            on_ok(expected.value());
            return ASR_S_OK;
        }
        else
        {
            return on_ok(expected.value());
        }
    }

    return expected.error();
}

template <class T, class OnOk, class OnError>
AsrResult Match(
    const ExpectedWithExplanation<T>& expected,
    OnOk&&                            on_ok,
    OnError&&                         on_error)
{
    if (expected)
    {
        if constexpr (std::is_same_v<
                          std::
                              invoke_result_t<OnOk, decltype(expected.value())>,
                          void>)
        {
            on_ok(expected.value());
            return ASR_S_OK;
        }
        else
        {
            return on_ok(expected.value());
        }
    }

    if constexpr (std::is_same_v<
                      std::invoke_result_t<OnError, decltype(expected.error())>,
                      void>)
    {
        auto& error = expected.error();
        on_error(error);
        return error.error_code;
    }
    else
    {
        return on_error(expected.error());
    }
}

ASR_UTILS_NS_END

template <>
struct ASR_FMT_NS::formatter<ASR::Utils::VariantString, char>
    : public formatter<const char*, char>
{
    auto format(const ASR::Utils::VariantString& string, format_context& ctx)
        const -> typename std::remove_reference_t<decltype(ctx)>::iterator;
};

#endif // ASR_UTILS_EXPECTED_H
