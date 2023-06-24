#include "ForeignInterfaceHost.h"
#include <AutoStarRail/Utils/UnexpectedEnumException.h>
#include <AutoStarRail/Utils/EnumUtils.hpp>
#include <AutoStarRail/AsrConfig.h>
#include <AutoStarRail/ExportInterface/IAsrSettings.h>
#include <AutoStarRail/Core/Logger/Logger.h>
#include <stdexcept>
#include <AutoStarRail/Core/ForeignInterfaceHost/AsrGuid.h>

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

namespace Details
{
    template <class T>
    void OptionalFromJson(const nlohmann::json& json, const char* key,
                          std::optional<T>& opt_value)
    {
        const auto it = json.find(key);
        if (it != json.end())
        {
            opt_value = it->template get<T>();
        }
        else
        {
            opt_value = std::nullopt;
        }
    }

    template <class T>
    void OptionalToJson(nlohmann::json& json, const char* key,
                        const std::optional<T>& opt_value)
    {
        if (opt_value)
        {
            json[key] = opt_value.value();
        }
    }
}

void from_json(const nlohmann::json& input, PluginSettingDesc& output)
{
    ASR_CORE_TRACE_SCOPE;

    output.name = input.at("name").get<std::string>();
    output.type = Utils::JsonToEnum<AsrType>(input, "type");
    switch (output.type)
    {
        using enum AsrType;
    case Bool:
        output.default_value = input.at("defaultValue").get<bool>();
        break;
    case Int:
        output.default_value = input.at("defaultValue").get<std::int64_t>();
        break;
    case Float:
        output.default_value = input.at("defaultValue").get<float>();
        break;
    case String:
        output.default_value = input.at("defaultValue").get<std::string>();
        break;
    default:
        throw Utils::UnexpectedEnumException::FromEnum(output.type);
    }
    Details::OptionalFromJson(input, "description", output.description);
    Details::OptionalFromJson(input, "enumValues", output.enum_values);
    Details::OptionalFromJson(input, "enumDescriptions",
                              output.enum_descriptions);
    Details::OptionalFromJson(input, "deprecation_message",
                              output.deprecation_message);
}

void from_json(const nlohmann::json& input, PluginDesc& output)
{
    ASR_CORE_TRACE_SCOPE;

    output.name = input.at("name").get<std::string>();
    output.author = input.at("author").get<std::string>();
    output.version = input.at("version").get<std::string>();
    const auto guid_string = input.at("guid").get<std::string>();
    output.guid = MakeAsrGuid(guid_string);
    input.at("settings").get_to(output.settings);
}

ASR_CORE_FOREIGNINTERFACEHOST_NS_END
