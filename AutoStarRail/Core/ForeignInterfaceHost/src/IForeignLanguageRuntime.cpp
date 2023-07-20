#include "IForeignLanguageRuntime.h"
#include "PythonHost.h"
#include "JavaHost.h"
#include "CppHost.h"
#include <AutoStarRail/Utils/UnexpectedEnumException.h>
#include <tl/expected.hpp>

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

auto CreateForeignLanguageRuntime(
    const ForeignLanguageRuntimeFactoryDescBase& desc_base)
    -> ASR::Utils::Expected<AsrPtr<IForeignLanguageRuntime>>
{
    switch (desc_base.language)
    {
        using enum ForeignInterfaceLanguage;
    case Python:
#ifndef ASR_EXPORT_PYTHON
        goto on_no_interface;
#else

#endif // ASR_EXPORT_PYTHON
    case CSharp:
#ifndef ASR_EXPORT_CSHARP
        goto on_no_interface;
#else
#endif // ASR_EXPORT_CSHARP
    case Java:
#ifndef ASR_EXPORT_JAVA
        goto on_no_interface;
#else
#endif // ASR_EXPORT_JAVA
    case Lua:
        goto on_no_interface;
    case Cpp:
        return CppHost::CreateForeignLanguageRuntime(desc_base);
    default:
        throw ASR::Utils::UnexpectedEnumException::FromEnum(desc_base.language);
    }
on_no_interface:
    return tl::make_unexpected(ASR_E_NO_IMPLEMENTATION);
}

ASR_CORE_FOREIGNINTERFACEHOST_NS_END