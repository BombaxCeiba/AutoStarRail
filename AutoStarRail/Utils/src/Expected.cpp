#include <AutoStarRail/Utils/Expected.h>

ASR_UTILS_NS_BEGIN

ASR_DEFINE_VARIABLE(Details::NULL_STRING){""};

ErrorAndExplanation::ErrorAndExplanation(const AsrResult error_code) noexcept
    : error_code{error_code}
{
}

ASR_UTILS_NS_END

ASR_NS_ANONYMOUS_DETAILS_BEGIN

template <class... Ts>
struct overload_set : Ts...
{
    using Ts::operator()...;
};

template <class... Ts>
overload_set(Ts...) -> overload_set<Ts...>;

ASR_NS_ANONYMOUS_DETAILS_END

auto ASR_FMT_NS::formatter<ASR::Utils::VariantString, char>::format(
    const ASR::Utils::VariantString& string,
    format_context&                  ctx) const ->
    typename std::remove_reference_t<decltype(ctx)>::iterator
{
    const char* p_string_data = std::visit(
        Details::overload_set{
            [](const char* string_in_variant) { return string_in_variant; },
            // match std::string_view and std::string
            [](const std::string_view string_in_variant)
            { return string_in_variant.data(); },
            [](const AsrString string_in_variant)
            { return string_in_variant.GetUtf8(); }},
        string);
    return ASR_FMT_NS::format_to(ctx.out(), "{}", p_string_data);
}
