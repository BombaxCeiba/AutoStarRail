#include "PluginFileManager.h"
#include <AutoStarRail/Core/GlobalSettingManager/GlobalSettingsManager.h>
#include <AutoStarRail/Utils/StringUtils.h>
#include "AutoStarRail/IAsrBase.h"
#include "AutoStarRail/Utils/Utils.hpp"
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

    boost::filesystem::path current_path{"./plugins"};
    for (const auto& it : boost::filesystem::directory_iterator(current_path))
    {
        const auto& it_path = it.path();
        const auto  extension = it.path().extension();
        if (std::wstring_view{extension.c_str()} == std::wstring_view{L"json"})
        {
            AsrResult     plugin_result{ASR_S_OK};
            std::ifstream plugin_config_stream{it_path.string()};
            if (!plugin_config_stream.is_open())
            {
                plugin_result = {};
                result.push_back(plugin_result);
                continue;
            }
            try
            {
                const auto config = nlohmann::json::parse(plugin_config_stream);
                const auto up_plugin_desc =
                    std::make_unique<PluginDesc>(config.get<PluginDesc>());

                if (up_plugin_desc->supported_system.find_first_of(
                        ASR_STR(ASR_PLATFORM))
                    != decltype(up_plugin_desc->supported_system)::npos)
                {
                    plugin_result = ASR_E_UNSUPPORTED_SYSTEM;
                };
            }
            catch (nlohmann::json::exception& ex)
            {
                ASR::Core::LogException(ex);
                plugin_result = ASR_E_INVALID_JSON;
            }
            result.push_back(plugin_result);
        }
    }
    return result;
}

ASR_CORE_FOREIGNINTERFACEHOST_NS_END