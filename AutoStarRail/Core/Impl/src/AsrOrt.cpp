#include <AutoStarRail/Core/Impl/AsrOrt.h>
#include <AutoStarRail/AsrString.hpp>
#include <boost/predef/os.h>

ASR_CORE_IMPL_NS_BEGIN

const ORTCHAR_T* ToOrtChar(AsrString string)
{
#if (BOOST_OS_WINDOWS)
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

ASR_DEFINE_VARIABLE(AsrOrt::default_cpu_memory_info) =
    Ort::MemoryInfo::CreateCpu(
        OrtAllocatorType::OrtArenaAllocator,
        OrtMemType::OrtMemTypeCPU);

AsrOrt::AsrOrt(const char* model_name)
    : env_{ORT_LOGGING_LEVEL_WARNING, model_name}
{
}

ASR_CORE_IMPL_NS_END