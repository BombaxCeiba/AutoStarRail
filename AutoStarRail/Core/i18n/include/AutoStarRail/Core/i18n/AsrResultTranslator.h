#ifndef ASR_CORE_I18N_ASRRESULTTRANSLATOR_H
#define ASR_CORE_I18N_ASRRESULTTRANSLATOR_H

#include <AutoStarRail/Core/i18n/i18n.h>

ASR_CORE_I18N_NS_BEGIN

AsrResult TranslateError(
    IAsrReadOnlyString*  local_name,
    AsrResult            error_code,
    IAsrReadOnlyString** out_string);

extern const I18n<AsrResult> g_translator_data;

ASR_CORE_I18N_NS_END

#endif // ASR_CORE_I18N_ASRRESULTTRANSLATOR_H