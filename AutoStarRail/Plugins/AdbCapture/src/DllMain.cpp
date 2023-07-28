#define ASR_BUILD_SHARED

#include "PluginImpl.h"

AsrResult AsrCoCreatePlugin(IAsrPlugin** pp_out_plugin)
{
    // TODO new a plugin object there.
    *pp_out_plugin = nullptr;
    return ASR_E_NO_IMPLEMENTATION;
}