#include "AdbCaptureImpl.h"
#include <array>

ASR_NS_BEGIN

int64_t AdbCapture::AddRef() { return ref_counter_.AddRef(); }

int64_t AdbCapture::Release() { return ref_counter_.Release(this); }

AsrResult AdbCapture::QueryInterface(const AsrGuid& iid, void** pp_object)
{
    auto& ref_p_object = *pp_object;
    if (iid == ASR_IID_BASE)
    {
        ref_p_object = static_cast<IAsrBase*>(this);
        return ASR_S_OK;
    }
    else if (iid == ASR_IID_CAPTURE)
    {
        ref_p_object = static_cast<IAsrCapture*>(this);
        return ASR_S_OK;
    }
    ref_p_object = nullptr;
    return ASR_E_NO_INTERFACE;
}

namespace
{
    const std::array<AsrCaptureType, 2> g_capture_descs = {
        {{sizeof(AsrCaptureType), 0, "Raw Adb", ""},
         {sizeof(AsrCaptureType), 1, "Adb With GZip", ""}}};
}

AsrResult
AdbCapture::GetAllAvailableCaptureType(const AsrCaptureType** p_type_array,
                                       unsigned int* count)
{
    *p_type_array = g_capture_descs.data();
    *count = g_capture_descs.size();
    return ASR_S_OK;
}

AsrResult AdbCapture::SetCaptureType(const unsigned int type)
{
    (void)type;
    return ASR_S_OK;
}

AsrResult AdbCapture::Capture(char** pp_rgba_data)
{
    *pp_rgba_data = nullptr;
    return -3;
}

ASR_NS_END