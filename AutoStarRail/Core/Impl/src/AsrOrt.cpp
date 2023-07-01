#include <AutoStarRail/Core/Impl/AsrOrt.h>
#include <AutoStarRail/AsrString.hpp>

ASR_CORE_IMPL_NS_BEGIN

const ORTCHAR_T* ToOrtChar(AsrString string)
{
#if (ASR_PLATFORM == ASR_CORE_IMPL_INTERNAL_WINDOWS)
    return string.GetW();
#else
    return string.GetUtf8();
#endif
}

const ORTCHAR_T* ToOrtChar(IAsrReadOnlyString* p_string)
{
    auto string = AsrString(p_string);
    return ToOrtChar(string);
}

AsrOrt::AsrOrt(const char* model_name)
    : env_{ORT_LOGGING_LEVEL_WARNING, model_name}
{
}

ASR_CORE_IMPL_NS_END