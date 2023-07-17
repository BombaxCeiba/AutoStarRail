#include <AutoStarRail/Core/i18n/AsrResultTranslator.h>
#include <AutoStarRail/Core/Logger/Logger.h>
#include <AutoStarRail/Utils/fmt.h>
#include <AutoStarRail/Utils/StringUtils.h>

ASR_CORE_I18N_NS_BEGIN

AsrResult TranslateError(
    IAsrReadOnlyString*  local_name,
    AsrResult            error_code,
    IAsrReadOnlyString** out_string)
{
    const char*    p_local_name{nullptr};
    const char8_t* p_u8_local_name{};
    std::u8string  u8_return_string{};

    auto result = local_name->GetUtf8(&p_local_name);
    if (!ASR::IsOk(result))
    {
        ASR_CORE_LOG_WARN(
            "Failed to get local name from string pointer. AsrCore will use \"en\" instead.");
        p_u8_local_name = u8"en";
    }
    else
    {
        p_u8_local_name = reinterpret_cast<const char8_t*>(p_local_name);
    }

    result = g_translator_data.GetErrorExplanation(
        p_u8_local_name,
        error_code,
        &u8_return_string);

    if (!ASR::IsOk(result))
    {
        const auto error_string = std::format(
            ASR_UTILS_STRINGUTILS_DEFINE_U8STR(
                "Error happened when getting error explanation. Code = {} ."),
            result);
        ASR_CORE_LOG_ERROR(error_string);
        AsrPtr<IAsrString> p_error_string;
        ::CreateIAsrStringFromUtf8(error_string.c_str(), p_error_string.Put());
        p_error_string->AddRef();
        *out_string = p_error_string.Get();
        return result;
    }

    AsrPtr<IAsrString> p_error_string;
    ::CreateIAsrStringFromUtf8(
        reinterpret_cast<const char*>(u8_return_string.c_str()),
        p_error_string.Put());
    p_error_string->AddRef();
    *out_string = p_error_string.Get();
    return result;
}

I18n<AsrResult> MakeAsrResultTranslatorData()
{
    TranslateResources<AsrResult> translate_resource{
        {u8"en",
         {{ASR_S_OK, u8"Success"},
          {ASR_E_NO_INTERFACE, u8"No interface"},
          {ASR_E_RETURN_VALUE_NOT_DEFINED, u8"Return value not defined"},
          {ASR_E_INVALID_STRING, u8"Invalid string"},
          {ASR_E_INVALID_STRING_SIZE, u8"Invalid string size"},
          {ASR_E_NO_IMPLEMENTATION, u8"No implementation"},
          {ASR_E_UNSUPPORTED_SYSTEM, u8"Unsupported system"},
          {ASR_E_INVALID_JSON, u8"Invalid JSON"}}},
        {
            u8"zh-cn",
            {{{ASR_S_OK, u8"成功"},
              {ASR_E_NO_INTERFACE, u8"接口未找到"},
              {ASR_E_RETURN_VALUE_NOT_DEFINED, u8"接口没有处理返回值"},
              {ASR_E_INVALID_STRING, u8"非法字符串"},
              {ASR_E_INVALID_STRING_SIZE, u8"非法字符串长度"},
              {ASR_E_NO_IMPLEMENTATION, u8"未实现"},
              {ASR_E_UNSUPPORTED_SYSTEM, u8"不支持的操作系统"},
              {ASR_E_INVALID_JSON, u8"非法的JSON数据"}}},
        }};
    decltype(g_translator_data) result{std::move(translate_resource)};
    return result;
}

ASR_DEFINE_VARIABLE(g_translator_data) = MakeAsrResultTranslatorData();

ASR_CORE_I18N_NS_END