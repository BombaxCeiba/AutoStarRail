#ifndef ASR_IPLUGIN_H
#define ASR_IPLUGIN_H

#include <AutoStarRail/IAsrBase.h>
#include <memory>

enum class AsrPluginFeature
{
    Capture,
    ErrorLens
};

// {09EA2A40-6A10-4756-AB2B-41B2FD75AB36}
ASR_DEFINE_GUID(
    ASR_IID_PLUGIN,
    IAsrPlugin,
    0x9ea2a40,
    0x6a10,
    0x4756,
    0xab,
    0x2b,
    0x41,
    0xb2,
    0xfd,
    0x75,
    0xab,
    0x36)
SWIG_IGNORE(IAsrPlugin)
ASR_INTERFACE IAsrPlugin : public IAsrBase
{
    ASR_METHOD EnumFeature(AsrPluginFeature * p_out_feature) = 0;
    ASR_METHOD GetCaptureInterface(
        const AsrGuid& feature_iid,
        IAsrBase**     pp_out_feature_interface) = 0;
};

ASR_RET_TYPE_DECLARE_BEGIN(AsrRetPluginFeature)
    AsrPluginFeature value;
ASR_RET_TYPE_DECLARE_END

ASR_RET_TYPE_DECLARE_BEGIN(AsrRetSwigBase)
    std::shared_ptr<IAsrSwigBase> value;
ASR_RET_TYPE_DECLARE_END

// {3F11FBB2-B19F-4C3E-9502-B6D7F1FF9DAA}
ASR_DEFINE_GUID(
    ASR_IID_SWIG_PLUGIN,
    IAsrSwigPlugin,
    0x3f11fbb2,
    0xb19f,
    0x4c3e,
    0x95,
    0x2,
    0xb6,
    0xd7,
    0xf1,
    0xff,
    0x9d,
    0xaa);
SWIG_ENABLE_DIRECTOR(IAsrSwigPlugin)
SWIG_ENABLE_SHARED_PTR(IAsrSwigPlugin)
ASR_INTERFACE IAsrSwigPlugin : public IAsrSwigBase
{
    virtual AsrRetPluginFeature EnumFeature() = 0;
    virtual AsrRetSwigBase GetCaptureInterface(const AsrGuid& feature_iid) = 0;
    virtual ~IAsrSwigPlugin() override = default;
};

#endif // ASR_IPLUGIN_H