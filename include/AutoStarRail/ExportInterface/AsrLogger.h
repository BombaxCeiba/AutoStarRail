#ifndef ASR_LOGGER_H
#define ASR_LOGGER_H

#include <AutoStarRail/AsrExport.h>
#include <AutoStarRail/AsrString.hpp>

#ifndef SWIG

ASR_C_API void AsrLogError(IAsrReadOnlyString* p_readonly_string);
ASR_C_API void AsrLogErrorU8(const char* p_string);

ASR_C_API void AsrLogWarning(IAsrReadOnlyString* p_readonly_string);
ASR_C_API void AsrLogWarningU8(const char* p_string);

ASR_C_API void AsrLogInfo(IAsrReadOnlyString* p_readonly_string);
ASR_C_API void AsrLogInfoU8(const char* p_string);

#endif // SWIG

// ASR_API void AsrLogError(AsrString asr_string);

// ASR_API void AsrLogWarning(AsrString asr_string);

// ASR_API void AsrLogInfo(AsrString asr_string);

#endif // ASR_LOGGER_H
