#ifndef ASR_UTILS_STRINGUTILS_HPP
#define ASR_UTILS_STRINGUTILS_HPP

#include <AutoStarRail/Utils/Config.h>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <vector>

#define ASR_UTILS_STRINGUTILS_COMPARE_STRING(var, string_literals)             \
    [&var]() -> bool                                                           \
    {                                                                          \
        constexpr auto rhs = string_literals;                                  \
        constexpr auto w_rhs = ASR_WSTR(string_literals);                      \
        return ASR::Utils::Compare(var, std::make_tuple(rhs, w_rhs));          \
    }()

#if ASR_P2513R4_SUPPORT
#define ASR_UTILS_STRINGUTILS_DEFINE_U8STR(string_literals)                    \
    []() noexcept                                                              \
    {                                                                          \
        const static char _asr_internal_result[] = ASR_U8STR(string_literals); \
        return _asr_internal_result;                                           \
    }()
#else
#define ASR_UTILS_STRINGUTILS_DEFINE_U8STR(string_literals)                       \
    []() noexcept                                                                 \
    {                                                                             \
        const static char8_t _asr_internal_result[] = ASR_U8STR(string_literals); \
        return reinterpret_cast<const char*>(_asr_internal_result);               \
    }()
#endif // ASR_HAVE_P2513R4_SUPPORT

ASR_UTILS_NS_BEGIN

template <class T>
concept is_method_c_str_existence = requires(const T& t) { t.c_str(); };

template <is_method_c_str_existence T>
bool Compare(const T& lhs, std::tuple<const char*, const wchar_t*> rhs)
{
    using CharT = std::remove_cv_t<
        std::remove_reference_t<std::remove_pointer_t<decltype(lhs.c_str())>>>;
    const auto lhs_view = std::basic_string_view<CharT>{lhs.c_str()};
    const auto rhs_value = std::get<const CharT*>(rhs);
    const auto rhs_view = decltype(lhs_view){rhs_value};
    return lhs_view == rhs_view;
}

void ToLowerInPlace(std::string& in_out_str);

void ToLowerInPlace(std::vector<std::string>& in_out_str_vector);

ASR_UTILS_NS_END

#endif // ASR_UTILS_STRINGUTILS_HPP
