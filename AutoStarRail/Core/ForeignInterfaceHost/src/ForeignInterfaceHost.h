#ifndef ASR_CORE_FOREIGNINTERFACEHOST_FOREIGNINTERFACEHOST_H
#define ASR_CORE_FOREIGNINTERFACEHOST_FOREIGNINTERFACEHOST_H

#include <string>
#include <vector>
#include <optional>
#include <variant>
#include <AutoStarRail/Core/ForeignInterfaceHost/Config.h>
#include <AutoStarRail/IAsrBase.h>
#include <AutoStarRail/ExportInterface/IAsrSettings.h>
#include <nlohmann/json_fwd.hpp>

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

struct PluginSettingDesc
{
    std::string name;
    std::variant<std::monostate, bool, std::int64_t, float, std::string>
        default_value;

    std::optional<std::string> description;
    std::optional<std::vector<std::string>> enum_values;
    std::optional<std::vector<std::string>> enum_descriptions;
    std::optional<std::string> deprecation_message;

    AsrType type = AsrType::String;
    /**
     * @brief 保留参数，不参与序列化
     *
     */
    // AsrSettingScope scope = AsrSettingScope::Global;
};

void from_json(const ::nlohmann::json& input, PluginSettingDesc& output);
// void to_json(const ::nlohmann::json& output, PluginSettingDesc& input);

struct PluginDesc
{
    std::string name;
    std::string author;
    std::string version;
    std::string supported_system;
    AsrGuid guid;
    std::vector<PluginSettingDesc> settings;
};

void from_json(const ::nlohmann::json& input, PluginDesc& output);
// void to_json(const ::nlohmann::json& output, PluginDesc& input);

ASR_CORE_FOREIGNINTERFACEHOST_NS_END

#endif // ASR_CORE_FOREIGNINTERFACEHOST_FOREIGNINTERFACEHOST_H