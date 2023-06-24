#ifndef ASR_CORE_FOREIGNINTERFACEHOST_PLUGINFILEMANAGER_H
#define ASR_CORE_FOREIGNINTERFACEHOST_PLUGINFILEMANAGER_H

#include <AutoStarRail/Core/ForeignInterfaceHost/Config.h>
#include "AutoStarRail/IAsrBase.h"
#include "AutoStarRail/PluginInterface/IAsrErrorLens.h"
#include "Plugin.h"
#include <map>
#include <boost/filesystem.hpp>
#include <vector>

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

struct PluginLoadError
{
    AsrResult error_code;
    std::string error_message;
};

class PluginFileManager
{
private:
    std::map<boost::filesystem::path, Plugin> plugin_file_paths{};

public:
    std::vector<AsrResult> Refresh();
};

ASR_CORE_FOREIGNINTERFACEHOST_NS_END

#endif // ASR_CORE_FOREIGNINTERFACEHOST_PLUGINFILEMANAGER_H
