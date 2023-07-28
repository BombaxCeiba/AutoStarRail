#ifndef ASR_LOGGER_H
#define ASR_LOGGER_H

#include <AutoStarRail/AsrExport.h>
#include <AutoStarRail/AsrString.hpp>

#ifndef SWIG

typedef struct _asr_SourceLocation
{
    const char* file_name;
    int         line;
    const char* function_name;
} AsrSourceLocation;

#ifdef __cplusplus
#ifdef _MSC_VER
#define ASR_INTERNAL_FUNCTION_NAME __FUNCSIG__
#else
#define ASR_INTERNAL_FUNCTION_NAME __PRETTY_FUNCTION__
#endif // _MSC_VER
#else
#define ASR_INTERNAL_FUNCTION_NAME __func__
#endif // __cplusplus

#define ASR_LOG_WITH_SOURCE_LOCATION(type, ...)                                \
    do                                                                         \
    {                                                                          \
        AsrSourceLocation _asr_internal_source_location = {                    \
            __FILE__,                                                          \
            __LINE__,                                                          \
            ASR_INTERNAL_FUNCTION_NAME};                                       \
        AsrLog##type##U8WithSourceLocation(                                    \
            __VA_ARGS__,                                                       \
            &_asr_internal_source_location);                                   \
    } while (true)

#define ASR_LOG_ERROR(...) ASR_LOG_WITH_SOURCE_LOCATION(Error, __VA_ARGS__)
#define ASR_LOG_WARNING(...) ASR_LOG_WITH_SOURCE_LOCATION(Warning, __VA_ARGS__)
#define ASR_LOG_INFO(...) ASR_LOG_WITH_SOURCE_LOCATION(Info, __VA_ARGS__)

ASR_C_API void AsrLogError(IAsrReadOnlyString* p_readonly_string);
ASR_C_API void AsrLogErrorU8(const char* p_string);
ASR_C_API void AsrLogErrorU8WithSourceLocation(
    const char*              p_string,
    const AsrSourceLocation* p_location);

ASR_C_API void AsrLogWarning(IAsrReadOnlyString* p_readonly_string);
ASR_C_API void AsrLogWarningU8(const char* p_string);
ASR_C_API void AsrLogWarningU8WithSourceLocation(
    const char*              p_string,
    const AsrSourceLocation* p_location);

ASR_C_API void AsrLogInfo(IAsrReadOnlyString* p_readonly_string);
ASR_C_API void AsrLogInfoU8(const char* p_string);
ASR_C_API void AsrLogInfoU8WithSourceLocation(
    const char*              p_string,
    const AsrSourceLocation* p_location);

#endif // SWIG

ASR_API void AsrLogError(AsrString asr_string);

ASR_API void AsrLogWarning(AsrString asr_string);

ASR_API void AsrLogInfo(AsrString asr_string);

#endif // ASR_LOGGER_H
