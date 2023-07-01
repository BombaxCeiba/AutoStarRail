#ifndef ASR_PLUGINS_ADBCAPTUREFACTORYIMPL_H
#define ASR_PLUGINS_ADBCAPTUREFACTORYIMPL_H

#include <AutoStarRail/PluginInterface/IAsrCapture.h>

class AdbCaptureFactoryImpl final : public IAsrCaptureFactory
{
    int64_t    AddRef() override;
    int64_t    Release() override;
    ASR_METHOD QueryInterface(const AsrGuid& riid, void** pp_object) override;

    ASR_METHOD CreateInstance(
        IAsrReadOnlyString* p_json_config,
        IAsrCapture**       pp_object) override;
};

#endif // ASR_PLUGINS_ADBCAPTUREFACTORYIMPL_H
