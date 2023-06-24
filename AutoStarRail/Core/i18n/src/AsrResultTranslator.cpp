#include <AutoStarRail/Core/i18n/AsrResultTranslator.h>

ASR_CORE_I18N_NS_BEGIN

ASR_DEFINE_VARIABLE(asr_result_translator) = []()
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
    decltype(asr_result_translator) result{std::move(translate_resource)};
    return result;
}();

ASR_CORE_I18N_NS_END