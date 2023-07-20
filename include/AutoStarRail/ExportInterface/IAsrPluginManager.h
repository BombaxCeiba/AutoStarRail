#ifndef ASR_PLUGINMANAGER_H
#define ASR_PLUGINMANAGER_H

/**
 * @file IAsrPluginManager.h
 * @brief The content in this file will NOT be processed by SWIG.
    The exported interface in this file should only be used by GUI programs.
 * @version 0.1
 * @date 2023-07-18
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "AutoStarRail/AsrExport.h"
#include "AutoStarRail/AsrString.hpp"
#include <AutoStarRail/IAsrBase.h>
#include <cstddef>

struct PluginInfo
{
    int32_t             plugin_metadata_version;
    IAsrReadOnlyString* name;
    IAsrReadOnlyString* description;
    IAsrReadOnlyString* author;
    IAsrReadOnlyString* version;
    IAsrReadOnlyString* supported_system;
    IAsrReadOnlyString* plugin_filename_extension;
    AsrGuid             guid;
};

// {B2678FF8-720C-48E6-AC00-77D43D08F580}
ASR_DEFINE_GUID(
    ASR_IID_PLUGIN_MANAGER,
    IAsrPluginManager,
    0xb2678ff8,
    0x720c,
    0x48e6,
    0xac,
    0x0,
    0x77,
    0xd4,
    0x3d,
    0x8,
    0xf5,
    0x80);
ASR_INTERFACE IAsrPluginManager : public IAsrBase
{
    AsrResult GetCount(size_t * p_out_size);
    /**
     * @brief Get the Info object. You must call the Release method on all
     * member variables of type IAsrReadOnlyString * to avoid Memory leak.
     *
     * @param index
     * @param p_out_info
     * @return AsrResult ASR_S_OK if success, ASR_E_OUT_OF_RANGE if index is out
     * of range.
     */
    AsrResult EnumInfo(size_t index, PluginInfo * p_out_info);
    AsrResult EnumLoadingResult(
        size_t index,
        AsrResult * p_out_error_code,
        IAsrReadOnlyString * *p_out_error_message);
};

/**
 * @brief Call this function to load all plugin.
 *
 * @param pp_out_result
 * @return ASR_C_API
 */
ASR_C_API AsrResult LoadPluginAndGetResult(
    IAsrGuidVector*     p_ignore_plugins_guid,
    IAsrPluginManager** pp_out_result);

#endif // ASR_PLUGINMANAGER_H
