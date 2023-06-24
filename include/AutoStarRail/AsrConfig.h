#ifndef ASR_ASRCONFIG_H
#define ASR_ASRCONFIG_H

#include "AsrExport.h"

#define ASR ::Asr

#define ASR_NS_BEGIN                                                           \
    namespace Asr                                                              \
    {

#define ASR_NS_END }

#define ASR_NS_ANONYMOUS_DETAILS_BEGIN                                         \
    namespace                                                                  \
    {                                                                          \
        namespace Details                                                      \
        {

#define ASR_NS_ANONYMOUS_DETAILS_END                                           \
    }                                                                          \
    }

#define ASR_PRAGMA(x) _Pragma

#define ASR_STR_IMPL(x) #x
#define ASR_STR(x) ASR_STR_IMPL(x)

#define ASR_CONCAT_IMPL(x, y) x##y
#define ASR_CONCAT(x, y) ASR_CONCAT_IMPL(x, y)

#define ASR_TOKEN_PASTE_IMPL(x, y) x##y
#define ASR_TOKEN_PASTE(x, y) ASR_TOKEN_PASTE_IMPL(x, y)

#define ASR_USTR(x) ASR_USTR_IMPL(x)
#define ASR_USTR_IMPL(x) u##x

#define ASR_U8STR(x) ASR_U8STR_IMPL(x)
#define ASR_U8STR_IMPL(x) u8##x

#define ASR_USING_BASE_CTOR(base) using base::base

#ifdef _MSC_VER
#define ASR_DISABLE_WARNING_BEGIN(x)                                           \
    ASR_PRAGMA(push)                                                           \
    ASR_PRAGMA(warning(disable : x))

#elif defined(__GNUC__)
#define ASR_DISABLE_WARNING_BEGIN(x)                                           \
    ASR_PRAGMA(GCC diagnostic push)                                            \
    ASR_PRAGMA(GCC diagnostic ignored x)

#elif defined(__clang__)
#define ASR_DISABLE_WARNING_BEGIN(x)                                           \
    ASR_PRAGMA(clang diagnostic push)                                          \
    ASR_PRAGMA(clang diagnostic ignored x)

#endif

#ifdef _MSC_VER
#define ASR_DISABLE_WARNING_END(x) ASR_PRAGMA(pop)

#elif defined(__GNUC__)
#define ASR_DISABLE_WARNING_END(x) ASR_PRAGMA(GCC diagnostic pop)

#elif defined(__clang__)
#define ASR_DISABLE_WARNING_END(x) ASR_PRAGMA(clang diagnostic pop)

#endif

#define ASR_DEFINE_VARIABLE(...) decltype(__VA_ARGS__) __VA_ARGS__

#endif // ASR_ASRCONFIG_H
