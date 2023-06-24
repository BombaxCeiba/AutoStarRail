#ifndef ASR_CORE_FOREIGNINTERFACEHOST_IFOREIGNLANGUAGERUNTIME_H
#define ASR_CORE_FOREIGNINTERFACEHOST_IFOREIGNLANGUAGERUNTIME_H

#include <filesystem>
#include <vector>
#include <string>
#include <AutoStarRail/IAsrBase.h>
#include "ForeignInterfaceHostEnum.h"

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

struct ForeignLanguageRuntimeFactoryDescBase
{
    std::filesystem::path runtime_dll_path;
    ForeignInterfaceLanguage language;
};

ASR_INTERFACE IForeignLanguageRuntime
{
    virtual AsrResult Init(
        const ForeignLanguageRuntimeFactoryDescBase& desc) = 0;
    virtual AsrResult LoadPlugin(const std::filesystem::path& path) = 0;
    virtual ~IForeignLanguageRuntime() = default;
};

AsrResult CreateForeignLanguageRuntime(
    const ForeignLanguageRuntimeFactoryDescBase& desc_base,
    IForeignLanguageRuntime** out_object);

ASR_CORE_FOREIGNINTERFACEHOST_NS_END

#endif // ASR_CORE_FOREIGNINTERFACEHOST_IFOREIGNLANGUAGERUNTIME_H
