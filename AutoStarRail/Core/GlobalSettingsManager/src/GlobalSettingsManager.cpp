#include <AutoStarRail/Utils/IAsrBaseAdapterUtils.h>
#include <AutoStarRail/Core/GlobalSettingManager/Config.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/AsrStringImpl.h>
#include <AutoStarRail/ExportInterface/IAsrSettings.h>
#include <AutoStarRail/Utils/Utils.hpp>
#include <AutoStarRail/Core/Logger/Logger.h>
#include <nlohmann/json.hpp>

ASR_CORE_GLOBALSETTINGSMANAGER_NS_BEGIN

ASR_NS_ANONYMOUS_DETAILS_BEGIN

template <class T, class OnTypeError, class OnJsonError>
T GetJsonValue(
    const nlohmann::json& j,
    const char*           key,
    OnTypeError           on_type_error,
    OnJsonError           on_json_error)
{
    T result{};
    try
    {
        result.value = j.at(key).get<decltype(T::value)>();
        result.error_code = ASR_S_OK;
    }
    catch (const nlohmann::json::type_error& ex)
    {
        result.error_code = ASR_E_TYPE_ERROR;
        on_type_error(ex, j, key);
    }
    catch (const nlohmann::json::exception& ex)
    {
        result.error_code = ASR_E_INVALID_JSON;
        on_json_error(ex, j, key);
    }
    return result;
}

constexpr auto default_on_type_error = [](const nlohmann::json::exception& ex,
                                          const nlohmann::json&            j,
                                          const char*                      key)
{ ASR_CORE_LOG_JSON_EXCEPTION(ex, key, j); };

constexpr auto default_on_json_error = [](const nlohmann::json::exception& ex,
                                          const nlohmann::json&            j,
                                          const char*                      key)
{ ASR_CORE_LOG_JSON_EXCEPTION(ex, key, j); };

template <class T>
T GetJsonValueOnDefaultErrorHandle(const nlohmann::json& j, const char* key)
{
    return GetJsonValue<T>(
        j,
        key,
        default_on_type_error,
        default_on_json_error);
}

ASR_NS_ANONYMOUS_DETAILS_END

class GlobalSettingsImpl
{
    nlohmann::json config_;

    GlobalSettingsImpl() = default;

public:
    static GlobalSettingsImpl& GetInstance()
    {
        static GlobalSettingsImpl instance{};
        return instance;
    }

    AsrResult IsCastAvailable(const AsrGuid& iid) const
    {
        if (ASR::Utils::
                IsMatchOneOf<&ASR_IID_SWIG_BASE, &ASR_IID_SWIG_SETTINGS>(iid))
        {
            return ASR_S_OK;
        }
        return ASR_E_NO_INTERFACE;
    }

    AsrRetString GetString(const char* key) const
    {
        return Details::GetJsonValueOnDefaultErrorHandle<AsrRetString>(
            config_,
            key);
    }

    AsrRetBool GetBool(const char* key) const
    {
        return Details::GetJsonValueOnDefaultErrorHandle<AsrRetBool>(
            config_,
            key);
    }

    AsrRetFloat GetFloat(const char* key) const
    {
        return Details::GetJsonValueOnDefaultErrorHandle<AsrRetFloat>(
            config_,
            key);
    }

    AsrRetInt GetInt(const char* key) const
    {
        return Details::GetJsonValueOnDefaultErrorHandle<AsrRetInt>(
            config_,
            key);
    }
};

class GlobalSettingsSwigAdapter final : public IAsrSwigSettings
{
    GlobalSettingsImpl& impl_;

public:
    GlobalSettingsSwigAdapter() : impl_{GlobalSettingsImpl::GetInstance()} {}
    ~GlobalSettingsSwigAdapter() override = default;

    AsrResult IsCastAvailable(const AsrGuid& iid) override
    {
        return impl_.IsCastAvailable(iid);
    }

    AsrRetString GetString(const AsrString key) override
    {
        return impl_.GetString(key.GetUtf8());
    }

    AsrRetBool GetBool(const AsrString key) override
    {
        return impl_.GetBool(key.GetUtf8());
    }

    AsrRetInt GetInt(const AsrString key) override
    {
        return impl_.GetInt(key.GetUtf8());
    }

    AsrRetFloat GetFloat(const AsrString key) override
    {
        return impl_.GetFloat(key.GetUtf8());
    }
};

class GlobalSettingsAdapter final : public IAsrSettings
{
    GlobalSettingsImpl& impl_;

public:
    GlobalSettingsAdapter() : impl_{GlobalSettingsImpl::GetInstance()} {}
    ~GlobalSettingsAdapter() = default;

    int64_t AddRef() override { return 1; }

    int64_t Release() override { return 1; }

    AsrResult QueryInterface(const AsrGuid& iid, void** ppv) override
    {
        return ASR::Utils::AsrInterfaceConverter(this, iid, ppv)
            .TryConvert<IAsrBase>()
            .TryConvert<IAsrSettings>()
            .GetResult();
    }

    AsrResult GetString(
        IAsrReadOnlyString*  key,
        IAsrReadOnlyString** pp_out_string) override
    {
        const auto asr_string = AsrString{key};
        const auto value = impl_.GetString(asr_string.GetUtf8());
        if (value.error_code == ASR_S_OK)
        {
            value.value.GetImpl(pp_out_string);
        }
        else
        {
            *pp_out_string = nullptr;
        }
        return value.error_code;
    }

    AsrResult GetBool(IAsrReadOnlyString* key, bool* p_out_bool) override
    {
        const auto asr_string = AsrString{key};
        const auto value = impl_.GetBool(asr_string.GetUtf8());
        if (value.error_code == ASR_S_OK)
        {
            *p_out_bool = value.value;
        }
        else
        {
            *p_out_bool = false;
        }
        return value.error_code;
    }

    AsrResult GetInt(IAsrReadOnlyString* key, int64_t* p_out_int) override
    {
        const auto asr_string = AsrString{key};
        const auto value = impl_.GetInt(asr_string.GetUtf8());
        if (value.error_code == ASR_S_OK)
        {
            *p_out_int = value.value;
        }
        else
        {
            *p_out_int = 0;
        }
        return value.error_code;
    }

    AsrResult GetFloat(IAsrReadOnlyString* key, float* p_out_float) override
    {
        const auto asr_string = AsrString{key};
        const auto value = impl_.GetFloat(asr_string.GetUtf8());
        if (value.error_code == ASR_S_OK)
        {
            *p_out_float = value.value;
        }
        else
        {
            *p_out_float = 0;
        }
        return value.error_code;
    }
};

GlobalSettingsAdapter g_settings_adapter{};

ASR_CORE_GLOBALSETTINGSMANAGER_NS_END

std::shared_ptr<IAsrSwigSettings> GetIAsrSwigSettings()
{
    auto result = std::make_shared<
        ASR::Core::GlobalSettingsManager::GlobalSettingsSwigAdapter>();
    return result;
}

AsrResult GetIAsrSettings(IAsrSettings** pp_settings)
{
    *pp_settings =
        std::addressof(ASR::Core::GlobalSettingsManager::g_settings_adapter);
    return ASR_S_OK;
}