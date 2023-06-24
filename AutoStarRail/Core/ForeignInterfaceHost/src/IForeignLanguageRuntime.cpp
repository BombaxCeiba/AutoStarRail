#include "IForeignLanguageRuntime.h"
#include "PythonHost.h"

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

AsrResult CreateForeignLanguageRuntime(
    const ForeignLanguageRuntimeFactoryDescBase& desc_base,
    IForeignLanguageRuntime** out_object)
{
    (void)desc_base;
    (void)out_object;
//     switch (desc_base.language)
//     {
//         using enum ForeignInterfaceLanguage;
//     case Python:
// #ifndef ASR_EXPORT_PYTHON
//         goto on_no_interface;
// #else

// #endif // ASR_EXPORT_PYTHON
//     case Java:
// #ifndef ASR_EXPORT_JAVA
//         goto on_no_interface;
// #else
// #endif // ASR_EXPORT_JAVA
//     }
// on_no_interface:
//     *out_object = nullptr;
    return ASR_E_NO_IMPLEMENTATION;
}

ASR_CORE_FOREIGNINTERFACEHOST_NS_END