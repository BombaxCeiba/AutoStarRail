#ifndef ASR_ITASK_H
#define ASR_ITASK_H

#include "AutoStarRail/AsrString.hpp"
#include <AutoStarRail/IAsrBase.h>

struct AsrDate
{
    uint16_t year;
    uint8_t  month;
    uint8_t  day;
    uint8_t  hour;
    uint8_t  minute;
    uint8_t  second;
};

enum class AsrTaskType
{
    Normal,
    StartUp,
    Login,
    Exit
};

ASR_DEFINE_GUID(
    ASR_IID_TASK,
    IAsrTask,
    0xace44315,
    0x637c,
    0x4e83,
    0xab,
    0xe0,
    0xda,
    0xa9,
    0xa,
    0x82,
    0xa6,
    0xca)
ASR_INTERFACE IAsrTask : public IAsrBase
{
    ASR_METHOD Do(
        IAsrReadOnlyString * p_connection_json,
        IAsrReadOnlyString * p_task_settings_json) = 0;
    ASR_METHOD GetNextExecutionTime(AsrDate * p_out_date) = 0;
    ASR_METHOD GetName(IAsrReadOnlyString * *pp_out_name) = 0;
    ASR_METHOD GetDescription(IAsrReadOnlyString * *pp_out_settings) = 0;
    ASR_METHOD GetLabel(IAsrReadOnlyString * *pp_out_label) = 0;
    ASR_METHOD GetType(AsrTaskType * p_out_type) = 0;
};

ASR_RET_TYPE_DECLARE_BEGIN(AsrRetDate)
    AsrDate value;
ASR_RET_TYPE_DECLARE_END

ASR_RET_TYPE_DECLARE_BEGIN(AsrRetTaskType)
    AsrTaskType value;
ASR_RET_TYPE_DECLARE_END

ASR_DEFINE_GUID(
    ASR_IID_SWIG_TASK,
    IAsrSwigTask,
    0xd4008fad,
    0xf64a,
    0x4388,
    0xae,
    0xcc,
    0x3e,
    0x29,
    0x2b,
    0x84,
    0xd2,
    0x30)
ASR_INTERFACE IAsrSwigTask : public IAsrSwigBase
{
    virtual AsrResult Do(
        AsrString connection_json,
        AsrString task_settings_json) = 0;
    virtual AsrRetDate     GetNextExecutionTime() = 0;
    virtual AsrRetString   GetName() = 0;
    virtual AsrRetString   GetDescription() = 0;
    virtual AsrRetString   GetLabel() = 0;
    virtual AsrRetTaskType GetType() = 0;
};

#endif // ASR_ITASK_H
