#ifndef ASR_CORE_FOREIGNINTERFACEHOST_PLUGIN_H
#define ASR_CORE_FOREIGNINTERFACEHOST_PLUGIN_H

#include <AutoStarRail/Core/ForeignInterfaceHost/Config.h>
#include <AutoStarRail/PluginInterface/IAsrPlugin.h>
#include <AutoStarRail/Core/Exceptions/InterfaceNotFoundException.h>
#include <AutoStarRail/AsrPtr.hpp>
#include <AutoStarRail/Utils/Utils.hpp>
#include <AutoStarRail/IAsrBase.h>
#include "ForeignInterfaceHost.h"
#include "ForeignInterfaceHostEnum.h"
#include "IForeignLanguageRuntime.h"
#include <stdexcept>
#include <memory>
#include <ctime>
#include <variant>

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

/**
 * @brief 尚不支持热重载
 *
 */
class Plugin
{
    friend class PluginManager;

    CommonPluginPtr                          p_plugin_{AsrPtr<IAsrPlugin>{}};
    std::unique_ptr<PluginDesc>              desc_;
    std::unique_ptr<IForeignLanguageRuntime> up_runtime_;
    ForeignInterfaceLanguage                 language_;

public:
    Plugin(
        AsrPtr<IAsrPlugin>                       p_plugin,
        std::unique_ptr<PluginDesc>              up_desc,
        std::unique_ptr<IForeignLanguageRuntime> up_runtime,
        ForeignInterfaceLanguage                 languange);

    ~Plugin();
};

ASR_CORE_FOREIGNINTERFACEHOST_NS_END

#endif // ASR_CORE_FOREIGNINTERFACEHOST_PLUGIN_H
