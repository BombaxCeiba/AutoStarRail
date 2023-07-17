#ifndef ASR_CORE_FOREIGNINTERFACEHOST_PLUGINFILEMANAGER_H
#define ASR_CORE_FOREIGNINTERFACEHOST_PLUGINFILEMANAGER_H

#include <AutoStarRail/Core/ForeignInterfaceHost/Config.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/AsrGuid.h>
#include <AutoStarRail/PluginInterface/IAsrCapture.h>
#include <AutoStarRail/PluginInterface/IAsrErrorLens.h>
#include <AutoStarRail/PluginInterface/IAsrTask.h>
#include <AutoStarRail/Core/Logger/Logger.h>
#include <AutoStarRail/Utils/fmt.h>
#include <AutoStarRail/Utils/Expected.h>
#include "Plugin.h"
#include <boost/filesystem.hpp>
#include <map>
#include <memory>
#include <variant>
#include <vector>

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

struct PluginLoadError
{
    AsrResult   error_code;
    std::string error_message;
};

class ErrorLensManager
{
public:
    using AsrErrorLensCommonPtr = std::
        variant<ASR::AsrPtr<IAsrErrorLens>, std::shared_ptr<IAsrSwigErrorLens>>;
    using AsrBaseCommonPtr =
        std::variant<ASR::AsrPtr<IAsrBase>, std::shared_ptr<IAsrSwigBase>>;

private:
    std::map<AsrBaseCommonPtr, AsrErrorLensCommonPtr> map_{};

    AsrResult InternalRegister(
        AsrBaseCommonPtr      p_asr_base,
        AsrErrorLensCommonPtr p_error_lens);
    AsrResult InternalUnregister(AsrBaseCommonPtr p_asr_base);

public:
    AsrResult Register(
        ASR::AsrPtr<IAsrBase> p_asr_base,
        AsrErrorLensCommonPtr p_error_lens);
    AsrResult Register(
        std::shared_ptr<IAsrSwigBase> p_asr_base,
        AsrErrorLensCommonPtr         p_error_lens);
    AsrResult UnRegister(ASR::AsrPtr<IAsrBase> p_asr_base);
    AsrResult UnRegister(std::shared_ptr<IAsrSwigBase> p_asr_base);
    auto      GetExplanation(
             AsrBaseCommonPtr    p_asr_base,
             IAsrReadOnlyString* local_name,
             AsrResult           error_code) const
        -> ASR::Utils::Expected<AsrPtr<IAsrReadOnlyString>>;
};

template <class T, class SwigT>
class InterfaceManager
{
private:
    struct PluginInterface
    {
        AsrPtr<T>              cpp_interface;
        std::shared_ptr<SwigT> swig_interface;
    };

    using GuidInterfaceMap = std::map<AsrGuid, PluginInterface>;

    GuidInterfaceMap map_;

    void InternalAddInterface(
        const PluginInterface& plugin_interface,
        const AsrGuid&         plugin_guid)
    {
        if (auto plugin_item = map_.find(plugin_guid);
            plugin_item != map_.end())
        {
            ASR_CORE_LOG_WARN(
                "Duplicate interface registration for plugin: {}",
                plugin_guid);
        }
        map_[plugin_guid] = plugin_interface;
    }

public:
    InterfaceManager() = default;

    AsrResult AddInterface(AsrPtr<T> p_interface, const AsrGuid& plugin_guid)
    {
        PluginInterface plugin_interface;
        plugin_interface.cpp_interface = p_interface;
        plugin_interface.swig_interface = CppToSwig<T>(p_interface);

        return InternalAddInterface(plugin_interface, plugin_guid);
    }

    AsrResult AddInterface(
        std::shared_ptr<SwigT> sp_interface,
        const AsrGuid&         plugin_guid)
    {
        PluginInterface plugin_interface;
        plugin_interface.cpp_interface = SwigToCpp<SwigT>(sp_interface);
        plugin_interface.swig_interface = sp_interface;

        return InternalAddInterface(plugin_interface, plugin_guid);
    }
};

class PluginManager
{
private:
    std::map<boost::filesystem::path, Plugin> plugin_file_paths_{};
    InterfaceManager<IAsrTask, IAsrSwigTask>  asr_task_interface_manager_;
    InterfaceManager<IAsrCapture, IAsrSwigCapture>
                     asr_capture_interface_manager_;
    ErrorLensManager error_lens_manager_;

    AsrResult AddInterface(ASR::AsrPtr<IAsrTask> p_task);
    AsrResult AddInterface(std::shared_ptr<IAsrSwigTask> sp_task);

    static std::unique_ptr<PluginDesc> GetPluginDesc(
        const boost::filesystem::path& metadata_path);

    AsrResult GetInterface(const Plugin& plugin);

public:
    std::vector<AsrResult> Refresh();

    /**
     * @brief Get the Error Explanation from AsrResult.
            Notice: This function will not hold ownership of local_name.
     * 
     * @param p_base 
     * @param local_name 
     * @param error_code 
     * @param pp_out_error_message 
     * @return AsrResult 
     */
    AsrResult GetErrorExplanation(
        ErrorLensManager::AsrBaseCommonPtr p_base,
        IAsrReadOnlyString*                local_name,
        AsrResult                          error_code,
        IAsrReadOnlyString**               pp_out_error_message) const;
};

extern PluginManager g_plugin_manager;

ASR_CORE_FOREIGNINTERFACEHOST_NS_END

#endif // ASR_CORE_FOREIGNINTERFACEHOST_PLUGINFILEMANAGER_H
