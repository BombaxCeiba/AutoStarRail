#include <AutoStarRail/Core/i18n/i18n.h>
#include <AutoStarRail/Utils/EnumUtils.hpp>
#include <nlohmann/json.hpp>
#include <string_view>
#include <fstream>
#include <type_traits>

ASR_CORE_I18N_NS_BEGIN

ASR_NS_ANONYMOUS_DETAILS_BEGIN

template <class T>
constexpr auto StringToTypeFactoryCheckOnCompileTime() noexcept
    -> T (*)(const std::string&)
{
    if constexpr (std::is_same_v<T, std::u8string>)
    {
        return [](const std::string& str) -> std::u8string {
            return {str.cbegin(), str.cend()};
        };
    }
    else if constexpr (std::is_same_v<T, std::string>)
    {
        return [](const std::string& str) -> std::string {
            return {str.cbegin(), str.cend()};
        };
    }
    else if constexpr (std::is_same_v<T, int> && (sizeof(T) >= sizeof(int)))
    {
        return [](const std::string& str) { return std::stoi(str); };
    }
    else if constexpr (std::is_same_v<T, float> && (sizeof(T) >= sizeof(float)))
    {
        return [](const std::string& str) { return std::stof(str); };
    }
}

ASR_NS_ANONYMOUS_DETAILS_END

template <class T>
auto StringToTypeFactory(AsrType type) -> T (*)(const std::string&)
{
    switch (type)
    {
    case AsrType::String:
        return Details::StringToTypeFactoryCheckOnCompileTime<T>();
    case AsrType::Int:
        return Details::StringToTypeFactoryCheckOnCompileTime<T>();
    case AsrType::Float:
        return Details::StringToTypeFactoryCheckOnCompileTime<T>();
    default:
        throw Utils::UnexpectedEnumException::FromEnum(type);
    }
}

template <class T>
void from_json(const ::nlohmann::json& input, TranslateJsonResource<T>& output)
{
    output.type = Utils::JsonToEnum<AsrType>(input, "type");
    const auto fn_string_to_type = StringToTypeFactory<T>(output.type);

    const auto& resources = input.at("resource");
    for (const auto& resource : resources)
    {
        TranslateItemMap<T> translate_item_map{};
        const auto          locale = resource.at("locale").get<std::u8string>();
        const auto          translate_items = resource.at("translateItems");
        for (const auto& [key, value] : translate_items.items())
        {
            auto t_key = fn_string_to_type(key);
            translate_item_map[t_key] = {value.begin(), value.end()};
        }
    }
}

template <class T>
I18n<T>::I18n(const boost::filesystem::path& json_path)
{
    std::ifstream ifs{};
    ifs.exceptions(std::ios::badbit | std::ios::failbit);
    ifs.open(json_path);
    const auto json = ::nlohmann::json::parse(ifs);
    translate_resource_ = json.get<decltype(translate_resource_)>();

    SetDefaultLocale(u8"en");
}

template <class T>
I18n<T>::I18n(const nlohmann::json& json)
{
    translate_resource_ = json.get<decltype(translate_resource_)>();
    SetDefaultLocale(u8"en");
}

template <class T>
I18n<T>::I18n(const TranslateResources<T>& translate_resource)
    : translate_resource_{translate_resource}
{
}

template <class T>
I18n<T>::I18n(TranslateResources<T>&& translate_resource)
    : translate_resource_{std::move(translate_resource)}
{
}

template <class T>
I18n<T>::~I18n() = default;

template <class T>
AsrResult I18n<T>::SetDefaultLocale(const char8_t* const default_locale)
{
    default_locale_ = std::u8string(default_locale);
    if (const auto it = translate_resource_.find(default_locale_);
        it != translate_resource_.end())
    {
        it_default_translate_map_ = it;
        return ASR_S_OK;
    }
    return ASR_E_NO_IMPLEMENTATION;
}

template <class T>
std::u8string I18n<T>::GetDefaultLocale() const
{
    return default_locale_;
}

template <class T>
AsrResult I18n<T>::GetErrorDescription(
    const T&       result,
    std::u8string* out_error_description) const
{
    const auto& translate_table = it_default_translate_map_->second;
    if (const auto it = translate_table.find(result);
        it != translate_table.end())
    {
        *out_error_description = it->second;
        return ASR_S_OK;
    }
    *out_error_description = {};
    return ASR_E_NO_IMPLEMENTATION;
}

ASR_CORE_I18N_NS_END