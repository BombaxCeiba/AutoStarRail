#ifndef ASR_PLUGINS_ADBCAPTUREIMPL_H
#define ASR_PLUGINS_ADBCAPTUREIMPL_H

#include "AutoStarRail/AsrConfig.h"
#include "AutoStarRail/IAsrBase.h"
#include <AutoStarRail/PluginInterface/IAsrCapture.h>
#include <AutoStarRail/Utils/Utils.hpp>
#include <cstdint>

ASR_NS_BEGIN

class AdbCapture final : public IAsrCapture
{
private:
    ASR::Utils::RefCounter<AdbCapture> ref_counter_{};

public:
    ~AdbCapture() = default;
    int64_t    AddRef() override;
    int64_t    Release() override;
    ASR_METHOD QueryInterface(const AsrGuid& riid, void** ppvObject) override;
    ASR_METHOD Capture(IAsrImage** pp_out_image) override;
};

ASR_NS_END

ASR_C_API IAsrCapture* IAsrCaptureFactory();

#endif
