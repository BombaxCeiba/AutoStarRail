#ifndef ASR_UTILS_FMT_H
#define ASR_UTILS_FMT_H

#include <AutoStarRail/AsrConfig.h>

#ifndef ASR_USE_STD_FMT
#define  ASR_USE_STD_FMT 1
#endif

#if ASR_USE_STD_FMT
#include <format>
ASR_NS_BEGIN
    namespace fmt {
        using namespace ::std;
    }
ASR_NS_END
#define ASR_FMT_NS std
#else
#include <fmt/core.h>
#include <fmt/format.h>
ASR_NS_BEGIN
    namespace fmt {
        using namespace ::fmt;
    }
ASR_NS_END
#define ASR_FMT_NS fmt
#endif

#endif // ASR_UTILS_FMT_H
