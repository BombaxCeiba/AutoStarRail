#include "PluginFileManager.h"
#include <AutoStarRail/Core/GlobalSettingManager/GlobalSettingsManager.h>
#include <AutoStarRail/Utils/StringUtils.h>
#include "AutoStarRail/IAsrBase.h"
#include "AutoStarRail/Utils/Utils.hpp"
#include <AutoStarRail/Utils/StreamUtils.hpp>
#include <AutoStarRail/Core/Logger/Logger.h>
#include "ForeignInterfaceHost.h"
#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include <fstream>
#include <nlohmann/json.hpp>
#include <magic_enum.hpp>

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

std::vector<AsrResult> PluginFileManager::Refresh()
{
    auto result = ASR::Utils::MakeEmptyContainer<std::vector<AsrResult>>();

    for (const auto  current_path = boost::filesystem::path{"./plugins"};
         const auto& it : boost::filesystem::directory_iterator{current_path})
    {
        const auto& it_path = it.path();
        const auto  extension = it.path().extension();
        if (ASR_UTILS_STRINGUTILS_COMPARE_STRING(extension, "json"))
        {
            AsrResult plugin_result{ASR_S_OK};

            auto plugin_config_stream = std::ifstream{};
            try
            {
                ASR::Utils::EnableStreamException(
                    plugin_config_stream,
                    std::ios::badbit | std::ios::failbit,
                    [&it_path](auto& stream) { stream.open(it_path.c_str()); });
                if (!plugin_config_stream.is_open())
                {
                    plugin_result = {};
                    result.push_back(plugin_result);
                    continue;
                }
                const auto config = nlohmann::json::parse(plugin_config_stream);
                const auto up_plugin_desc =
                    std::make_unique<PluginDesc>(config.get<PluginDesc>());

                const auto CURRENT_PLATFORM =
                    ASR_UTILS_STRINGUTILS_DEFINE_U8STR(ASR_STR(ASR_PLATFORM));

                if (up_plugin_desc->supported_system.find_first_of(
                        CURRENT_PLATFORM)
                    != decltype(up_plugin_desc->supported_system)::npos)
                {
                    plugin_result = ASR_E_UNSUPPORTED_SYSTEM;
                };
            }
            catch (const nlohmann::json::exception& ex)
            {
                ASR::Core::LogException(ex);
                plugin_result = ASR_E_INVALID_JSON;
            }
            catch (const std::ios_base::failure& ex)
            {
                ASR::Core::LogException(ex);
                plugin_result = ASR_E_INVALID_FILE;
            }
            result.push_back(plugin_result);
        }
    }
    return result;
}

ASR_CORE_FOREIGNINTERFACEHOST_NS_END