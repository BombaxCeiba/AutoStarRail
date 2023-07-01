#include "AdbCaptureImpl.h"
#include <AutoStarRail/Utils/IAsrBaseAdapterUtils.h>
#include <array>

ASR_NS_BEGIN

int64_t AdbCapture::AddRef() { return ref_counter_.AddRef(); }

int64_t AdbCapture::Release() { return ref_counter_.Release(this); }

AsrResult AdbCapture::QueryInterface(const AsrGuid& iid, void** pp_object)
{
    return ASR::Utils::AsrInterfaceConverter(this, iid, pp_object)
        .IsExpected<IAsrBase>()
        .IsExpected<IAsrCapture>()
        .GetResult();
}

AsrResult AdbCapture::Capture(IAsrImage** pp_out_image)
{
    (void)pp_out_image;
    return ASR_E_NO_IMPLEMENTATION;
}

ASR_NS_END