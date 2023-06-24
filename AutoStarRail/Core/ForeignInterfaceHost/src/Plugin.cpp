#include "Plugin.h"
#include "ForeignInterfaceHost.h"
#include <format>

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

Plugin::Plugin(
    IAsrPlugin*                              p_impl,
    std::unique_ptr<IForeignLanguageRuntime> up_runtime,
    std::unique_ptr<PluginDesc>              up_plugin_desc,
    ForeignInterfaceLanguage                 language)
    : p_impl_{p_impl}, up_runtime_{std::move(up_runtime)},
      up_plugin_desc_{std::move(up_plugin_desc)}, language_{language}
{
}

ASR_CORE_FOREIGNINTERFACEHOST_NS_END