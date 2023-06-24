#ifndef ASR_STEEINGS_H
#define ASR_STEEINGS_H

#include "AutoStarRail/AsrPtr.hpp"
#include <AutoStarRail/IAsrBase.h>
#include <AutoStarRail/AsrString.hpp>
#include <memory>

enum class AsrType : uint32_t
{
    Int,
    Float,
    String,
    Bool
};

// {6180A529-2C54-4EA1-A6D0-892682662DD2}
ASR_DEFINE_GUID(
    ASR_IID_SETTINGS,
    IAsrSettings,
    0x6180a529,
    0x2c54,
    0x4ea1,
    0xa6,
    0xd0,
    0x89,
    0x26,
    0x82,
    0x66,
    0x2d,
    0xd2);
SWIG_IGNORE(IAsrSettings)
ASR_INTERFACE IAsrSettings : public IAsrBase
{
    ASR_METHOD GetString(
        IAsrReadOnlyString * key,
        IAsrReadOnlyString * *pp_out_string) = 0;
    ASR_METHOD GetBool(IAsrReadOnlyString * key, bool* pp_out_bool) = 0;
    ASR_METHOD GetInt(IAsrReadOnlyString * key, int64_t * pp_out_int) = 0;
    ASR_METHOD GetFloat(IAsrReadOnlyString * key, float* pp_out_float) = 0;
};

// {0552065B-8FDF-46C7-82BA-703665E769EF}
ASR_DEFINE_GUID(
    ASR_IID_SWIG_SETTINGS,
    IAsrSwigSettings,
    0x552065b,
    0x8fdf,
    0x46c7,
    0x82,
    0xba,
    0x70,
    0x36,
    0x65,
    0xe7,
    0x69,
    0xef)
SWIG_ENABLE_SHARED_PTR(IAsrSwigSettings)
ASR_INTERFACE IAsrSwigSettings : public IAsrSwigBase
{
    virtual AsrRetString GetString(const AsrString key) = 0;
    virtual AsrRetBool   GetBool(const AsrString key) = 0;
    virtual AsrRetInt    GetInt(const AsrString key) = 0;
    virtual AsrRetFloat  GetFloat(const AsrString key) = 0;
};

ASR_API std::shared_ptr<IAsrSwigSettings> GetIAsrSwigSettings();

SWIG_IGNORE(GetIAsrSettings)
ASR_C_API AsrResult GetIAsrSettings(IAsrSettings** pp_settings);

#endif // ASR_STEEINGS_H
