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
    AdbCapture(const AsrCaptureType& desc);
    ~AdbCapture() = default;
    int64_t AddRef() override;
    int64_t Release() override;
    ASR_METHOD QueryInterface(const AsrGuid& riid, void** ppvObject) override;
    ASR_METHOD Capture(char** pp_rgba_data) override;
    ASR_METHOD
    GetAllAvailableCaptureType(const AsrCaptureType** p_type_array,
                               unsigned int* count) override;
    ASR_METHOD SetCaptureType(const unsigned int type) override;
};

ASR_NS_END

ASR_C_API IAsrCapture* IAsrCaptureFactory();

#endif
