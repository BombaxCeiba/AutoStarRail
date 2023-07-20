#ifndef ASR_TASKMANAGER_H
#define ASR_TASKMANAGER_H

/**
 * @file IAsrTaskManager.h
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

struct AsrTaskInfo
{
};

// {23B3F3A7-40E4-4A04-B1F0-9F2F15B8775C}
ASR_DEFINE_GUID(
    ASR_IID_TASK_MANAGER,
    IAsrTaskManager,
    0x23b3f3a7,
    0x40e4,
    0x4a04,
    0xb1,
    0xf0,
    0x9f,
    0x2f,
    0x15,
    0xb8,
    0x77,
    0x5c);
ASR_INTERFACE IAsrTaskManager : public IAsrBase
{
    ASR_METHOD SetEnable(const AsrGuid& plugin_id);

    ASR_METHOD EnumTask(size_t index, AsrTaskInfo * p_out_info);

    ASR_METHOD Resume(IAsrReadOnlyString * *pp_out_error_string);
    ASR_METHOD Pause(IAsrReadOnlyString * *pp_out_error_string);

    ASR_METHOD UpdateConnectionJson(
        IAsrReadOnlyString * p_connection_json,
        IAsrReadOnlyString * *pp_out_error_string);
};

ASR_C_API AsrResult CreateIAsrTaskManager(
    IAsrReadOnlyString* p_connection_json,
    IAsrTaskManager**   ppIAsrTaskManager);

#endif // ASR_TASKMANAGER_H
