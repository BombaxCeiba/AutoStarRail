#ifndef ASR_ICAPTURE_H
#define ASR_ICAPTURE_H

#include <AutoStarRail/IAsrBase.h>
#include <cstddef>
#include <cstdint>

class IAsrReadOnlyString;

// {69A9BDB0-4657-45B6-8ECB-E4A8F0428E95}
ASR_DEFINE_GUID(
    ASR_IID_CAPTURE,
    IAsrCapture,
    0x69a9bdb0,
    0x4657,
    0x45b6,
    0x8e,
    0xcb,
    0xe4,
    0xa8,
    0xf0,
    0x42,
    0x8e,
    0x95);
SWIG_IGNORE(IAsrCapture)
ASR_INTERFACE IAsrCapture : public IAsrBase
{
    /**
     * @brief Capture rbga data.
     *
     * @param pp_rgba_data the caller will take memory ownership of this
     * pointer.
     * @return ASR_METHOD
     */
    ASR_METHOD Capture(char** pp_rgba_data) = 0;
};

// {35264072-8F42-46B5-99EA-3A83E0227CF9}
ASR_DEFINE_GUID(
    ASR_IID_CAPTURE_FACTORY,
    IAsrCaptureFactory,
    0x35264072,
    0x8f42,
    0x46b5,
    0x99,
    0xea,
    0x3a,
    0x83,
    0xe0,
    0x22,
    0x7c,
    0xf9);
SWIG_IGNORE(IAsrCaptureFactory)
ASR_INTERFACE IAsrCaptureFactory : public IAsrBase
{
    /**
     * @brief Create a Instance object
     *
     * @param p_json_string
     * @param pp_object
     * @return ASR_METHOD
     */
    ASR_METHOD CreateInstance(
        IAsrReadOnlyString * p_json_string,
        IAsrCapture * *pp_object);
};

#endif // ASR_ICAPTURE_H
