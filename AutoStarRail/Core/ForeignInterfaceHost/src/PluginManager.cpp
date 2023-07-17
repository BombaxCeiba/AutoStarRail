#include "PluginManager.h"
#include <AutoStarRail/Core/GlobalSettingManager/GlobalSettingsManager.h>
#include <AutoStarRail/Utils/StringUtils.h>
#include <AutoStarRail/AsrString.hpp>
#include <AutoStarRail/IAsrBase.h>
#include <AutoStarRail/PluginInterface/IAsrTask.h>
#include <AutoStarRail/Utils/Utils.hpp>
#include <AutoStarRail/Utils/StreamUtils.hpp>
#include <AutoStarRail/Core/Logger/Logger.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/CppSwigInterop.h>
#include <AutoStarRail/Core/i18n/AsrResultTranslator.h>
#include "AutoStarRail/Utils/Expected.h"
#include "ForeignInterfaceHost.h"
#include <cstddef>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <fstream>
#include <nlohmann/json.hpp>
#include <magic_enum.hpp>

AsrResult GetErrorExplanation(
    IAsrBase*            p_error_generating_interface,
    IAsrReadOnlyString*  local_name,
    AsrResult            error_code,
    IAsrReadOnlyString** pp_out_error_explanation)
{
    Asr::AsrPtr<IAsrReadOnlyString> p_local_name_holder{local_name};
    return Asr::Core::ForeignInterfaceHost::g_plugin_manager
        .GetErrorExplanation(
            {ASR::AsrPtr(p_error_generating_interface)},
            p_local_name_holder.Get(),
            error_code,
            pp_out_error_explanation);
}

AsrRetString GetErrorExplanation(
    std::shared_ptr<IAsrSwigBase> p_error_generating_interface,
    AsrString                     local_name,
    AsrResult                     error_code)
{
    AsrRetString                    result;
    ASR::AsrPtr<IAsrReadOnlyString> p_local_name{};
    IAsrReadOnlyString*             p_result{nullptr};

    local_name.GetImpl(p_local_name.Put());
    result.error_code =
        ASR::Core::ForeignInterfaceHost::g_plugin_manager.GetErrorExplanation(
            {p_error_generating_interface},
            p_local_name.Get(),
            error_code,
            &p_result);
    result.value = AsrString{p_result};
    return result;
}

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

ASR_NS_ANONYMOUS_DETAILS_BEGIN

struct _asr_internal_GetPluginFeatureContext
{
    const size_t      index;
    AsrResult&        out_enum_result;
    AsrPluginFeature& out_feature;
};

template <class T>
constexpr auto MakePluginFeatureGetter()
{
    if constexpr (std::is_same_v<AsrPtr<IAsrPlugin>, T>)
    {
        return [](T& p_plugin, _asr_internal_GetPluginFeatureContext context)
        {
            context.out_enum_result =
                p_plugin->EnumFeature(context.index, &context.out_feature);
            return context.out_enum_result == ASR_S_OK;
        };
    }
    else if constexpr (std::is_same_v<std::shared_ptr<IAsrSwigPlugin>, T>)
    {
        return [](T& p_plugin, _asr_internal_GetPluginFeatureContext context)
        {
            const auto swig_enum_result = p_plugin->EnumFeature(context.index);
            context.out_feature = swig_enum_result.value;
            context.out_enum_result = swig_enum_result.error_code;
            return context.out_enum_result == ASR_S_OK;
        };
    }
}

template <class T>
auto GetSupportedFeatures(T& p_plugin, std::string_view plugin_name)
    -> ASR::Utils::ExpectedWithExplanation<std::vector<AsrPluginFeature>>
{
    std::vector<AsrPluginFeature> result{};
    constexpr size_t              max_enum_count =
        magic_enum::enum_count<AsrPluginFeature>() + 10;

    size_t index = 0;

    AsrPluginFeature feature{};
    AsrResult        enum_feature_result{ASR_E_RETURN_VALUE_NOT_DEFINED};

    constexpr auto checker = MakePluginFeatureGetter<T>();

    for (; checker(p_plugin, {index, enum_feature_result, feature}); index++)
    {
        if (index < max_enum_count)
        {
            result.push_back(feature);
        }
        else
        {
            return ASR::Utils::MakeUnexpected(
                ASR_E_MAYBE_OVERFLOW,
                ASR::fmt::format(
                    "Executing function \"EnumFeature\" in plugin(name = {}) more than the maximum limit of {} times, stopping.",
                    plugin_name,
                    max_enum_count));
        }
    }

    if (enum_feature_result != ASR_E_OUT_OF_RANGE)
    {
        // TODO: Call function below when unexpected.
        // LogErrorWhenGettingPluginFeature(enum_feature_result, plugin_name)
        return ASR::Utils::MakeUnexpected(
            enum_feature_result,
            ASR::fmt::format(
                "Getting plugin(name = {}) feature failed with error code {}.",
                plugin_name,
                enum_feature_result));
    }

    return {result};
}

template <class T>
auto GetSupportedInterface(
    T&                                   p_plugin,
    const std::vector<AsrPluginFeature>& features)
{
    for (auto feature : features)
    {
        AsrPtr<IAsrBase> p_interface{};
        auto             get_interface_result =
            p_plugin->GetInterface(feature, &p_interface);
        if (!ASR::IsOk(get_interface_result))
        {
            // TODO: Call internal error lens to interpret the error code.
            ASR_CORE_LOG_ERROR(
                "Get plugin interface for feature {}(value={}) failed: {}",
                magic_enum::enum_name(feature),
                feature,
                get_interface_result);
        }
    }
}

template <class T>
auto CallErrorLens(
    T&                  p_error_lens,
    IAsrReadOnlyString* local_name,
    AsrResult error_code) -> ASR::Utils::Expected<AsrPtr<IAsrReadOnlyString>>
{
    if constexpr (std::is_same_v<AsrPtr<IAsrErrorLens>, T>)
    {
        AsrResult                  call_result{ASR_E_RETURN_VALUE_NOT_DEFINED};
        AsrPtr<IAsrReadOnlyString> p_explanation{};
        call_result = p_error_lens->TranslateError(
            local_name,
            error_code,
            p_explanation.Put());
        if (ASR::IsOk(call_result))
        {
            return {p_explanation};
        }
        else
        {
            return tl::make_unexpected(call_result);
        }
    }
    else if constexpr (std::is_same_v<std::shared_ptr<IAsrSwigErrorLens>, T>)
    {
        auto swig_result = p_error_lens->TranslateError(local_name, error_code);
        if (ASR::IsOk(swig_result.error_code))
        {
            return {swig_result.value};
        }
        else
        {
            return tl::make_unexpected(swig_result.error_code);
        }
    }
    return tl::make_unexpected(ASR_E_RETURN_VALUE_NOT_DEFINED);
}

ASR_NS_ANONYMOUS_DETAILS_END

AsrResult ErrorLensManager::InternalRegister(
    AsrBaseCommonPtr      p_asr_base,
    AsrErrorLensCommonPtr p_error_lens)
{
    const auto it = map_.find(p_asr_base);
    if (it != map_.end())
    {
        return ASR_E_DUPLICATE_ELEMENT;
    }
    else
    {
        map_[p_asr_base] = p_error_lens;
        return ASR_S_OK;
    }
}

AsrResult ErrorLensManager::InternalUnregister(AsrBaseCommonPtr p_asr_base)
{
    const auto it = map_.find(p_asr_base);
    if (it != map_.end())
    {
        map_.erase(it);
        return ASR_S_OK;
    }
    else
    {
        return ASR_E_OUT_OF_RANGE;
    }
}

AsrResult ErrorLensManager::Register(
    ASR::AsrPtr<IAsrBase> p_asr_base,
    AsrErrorLensCommonPtr p_error_lens)
{
    return InternalRegister({p_asr_base}, p_error_lens);
}

AsrResult ErrorLensManager::Register(
    std::shared_ptr<IAsrSwigBase> p_asr_base,
    AsrErrorLensCommonPtr         p_error_lens)
{
    return InternalRegister({p_asr_base}, p_error_lens);
}

AsrResult ErrorLensManager::UnRegister(ASR::AsrPtr<IAsrBase> p_asr_base)
{
    return InternalUnregister({p_asr_base});
}

AsrResult ErrorLensManager::UnRegister(std::shared_ptr<IAsrSwigBase> p_asr_base)
{
    return InternalUnregister({p_asr_base});
}

auto ErrorLensManager::GetExplanation(
    AsrBaseCommonPtr    p_asr_base,
    IAsrReadOnlyString* local_name,
    AsrResult           error_code) const
    -> ASR::Utils::Expected<AsrPtr<IAsrReadOnlyString>>
{
    // 不是插件自定义错误时
    if (error_code < ASR_E_RESERVED)
    {
        AsrPtr<IAsrReadOnlyString> p_error_explanation{};
        i18n::TranslateError(local_name, error_code, p_error_explanation.Put());
        return {p_error_explanation};
    }
    if (auto it = map_.find(p_asr_base); it != map_.end())
    {
        return std::visit(
            [local_name, error_code](auto& p_error_lens) {
                return Details::CallErrorLens(
                    p_error_lens,
                    local_name,
                    error_code);
            },
            it->second);
    }
    else
    {
        return tl::make_unexpected(ASR_E_NO_INTERFACE);
    }
}

ASR_DEFINE_VARIABLE(g_plugin_manager){};

AsrResult PluginManager::AddInterface(ASR::AsrPtr<IAsrTask> p_task)
{
    (void)p_task;
    return ASR_E_NO_IMPLEMENTATION;
}

std::unique_ptr<PluginDesc> PluginManager::GetPluginDesc(
    const boost::filesystem::path& metadata_path)
{
    std::unique_ptr<PluginDesc> result{};
    std::ifstream               plugin_config_stream{};

    ASR::Utils::EnableStreamException(
        plugin_config_stream,
        std::ios::badbit | std::ios::failbit,
        [&metadata_path](auto& stream) { stream.open(metadata_path.c_str()); });

    const auto config = nlohmann::json::parse(plugin_config_stream);
    result = std::make_unique<PluginDesc>(config.get<PluginDesc>());

    return result;
}

AsrResult PluginManager::GetInterface(const Plugin& plugin)
{
    (void)plugin;
    return ASR_E_NO_IMPLEMENTATION;
}

std::vector<AsrResult> PluginManager::Refresh()
{
    auto result = ASR::Utils::MakeEmptyContainer<std::vector<AsrResult>>();
    result.reserve(50);

    for (const auto  current_path = boost::filesystem::path{"./plugins"};
         const auto& it : boost::filesystem::directory_iterator{current_path})
    {
        const auto& it_path = it.path();
        const auto  extension = it.path().extension();
        if (ASR_UTILS_STRINGUTILS_COMPARE_STRING(extension, "json"))
        {
            AsrResult plugin_result{ASR_S_OK};

            try
            {
                const auto up_plugin_desc = GetPluginDesc(it_path);

                if (const auto& CURRENT_PLATFORM =
                        ASR_UTILS_STRINGUTILS_DEFINE_U8STR(
                            ASR_STR(ASR_PLATFORM));
                    up_plugin_desc->supported_system.find_first_of(
                        CURRENT_PLATFORM)
                    != decltype(up_plugin_desc->supported_system)::npos)
                {
                    result.push_back(ASR_E_UNSUPPORTED_SYSTEM);
                    ASR_CORE_LOG_INFO(
                        "Error when checking system requirement. Error code (" ASR_STR(
                            ASR_E_UNSUPPORTED_SYSTEM) "): {}",
                        result.back());
                    continue;
                }

                boost::filesystem::path plugin_path =
                    it_path.parent_path()
                    / boost::filesystem::path{
                        it_path.stem().string()
                        + up_plugin_desc->plugin_filename_extension};
                if (!boost::filesystem::exists(plugin_path))
                {
                    result.push_back(ASR_E_FILE_NOT_FOUND);
                    ASR_CORE_LOG_ERROR(
                        "Error when checking plugin file. Expected file path:\"{}\". Error code (" ASR_STR(
                            ASR_E_FILE_NOT_FOUND) "): {}",
                        plugin_path.string(),
                        result.back());
                    continue;
                }
            }
            catch (const nlohmann::json::exception& ex)
            {
                ASR_CORE_LOG_EXCEPTION(ex);
                plugin_result = ASR_E_INVALID_JSON;
            }
            catch (const std::ios_base::failure& ex)
            {
                ASR_CORE_LOG_EXCEPTION(ex);
                plugin_result = ASR_E_INVALID_FILE;
            }
            result.push_back(plugin_result);
        }
    }
    return result;
}

AsrResult PluginManager::GetErrorExplanation(
    ErrorLensManager::AsrBaseCommonPtr p_base,
    IAsrReadOnlyString*                local_name,
    AsrResult                          error_code,
    IAsrReadOnlyString**               pp_out_error_message) const
{
    return ASR::Utils::Match(
        error_lens_manager_.GetExplanation(p_base, local_name, error_code),
        [&pp_out_error_message](const AsrPtr<IAsrReadOnlyString>& p_string)
        {
            p_string->AddRef();
            *pp_out_error_message = p_string.Get();
        });
}

ASR_CORE_FOREIGNINTERFACEHOST_NS_END