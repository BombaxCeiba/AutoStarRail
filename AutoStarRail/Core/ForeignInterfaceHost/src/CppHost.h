#ifndef ASR_CORE_FOREIGNINTERFACEHOST_CPPINTERFACE_H
#define ASR_CORE_FOREIGNINTERFACEHOST_CPPINTERFACE_H

#include <AutoStarRail/Core/ForeignInterfaceHost/Config.h>
#include "Plugin.h"

#define ASR_NS_CPPHOST_BEGIN                                                   \
    namespace CppHost                                                          \
    {

#define ASR_NS_CPPHOST_END }

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

ASR_NS_CPPHOST_BEGIN

auto CreateForeignLanguageRuntime(
    const ForeignLanguageRuntimeFactoryDescBase& desc)
    -> ASR::Utils::Expected<AsrPtr<IForeignLanguageRuntime>>;

ASR_NS_CPPHOST_END

ASR_CORE_FOREIGNINTERFACEHOST_NS_END

#endif // ASR_CORE_FOREIGNINTERFACEHOST_CPPINTERFACE_H
