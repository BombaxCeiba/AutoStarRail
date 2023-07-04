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

ASR_UTILS_NS_BEGIN

template <class T>
bool Compare(const T* lhs, std::tuple<const char*, const wchar_t*> rhs)
{
    const auto lhs_view = std::basic_string_view<T>{lhs};
    const auto rhs_value = std::get<const T*>(rhs);
    const auto rhs_view = decltype(lhs_view){rhs_value};
    return lhs_view == rhs_view;
}

void ToLowerInPlace(std::string& in_out_str);

void ToLowerInPlace(std::vector<std::string>& in_out_str_vector);

ASR_UTILS_NS_END

#endif // ASR_UTILS_STRINGUTILS_HPP
