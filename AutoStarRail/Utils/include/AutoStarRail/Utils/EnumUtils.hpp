#ifndef ASR_UTILS_ENUMTUILS_HPP
#define ASR_UTILS_ENUMTUILS_HPP

#include <AutoStarRail/AsrConfig.h>
#include <AutoStarRail/Utils/UnexpectedEnumException.h>
#include <magic_enum.hpp>
#include <nlohmann/json.hpp>
#include <string_view>

ASR_UTILS_NS_BEGIN

template <class Enum>
Enum StringToEnum(const std::string_view string)
{
    const auto opt_value = magic_enum::enum_cast<Enum>(string);
    if (!opt_value)
    {
        throw UnexpectedEnumException(string);
    }
    return opt_value.value();
}

template <class Enum>
Enum JsonToEnum(const nlohmann::json& json, const char* key)
{
    const auto string = json.at(key).get<std::string>();
    return StringToEnum<Enum>(string);
}

ASR_UTILS_NS_END

#endif // ASR_UTILS_ENUMTUILS_HPP