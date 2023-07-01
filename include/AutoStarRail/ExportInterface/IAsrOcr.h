#ifndef ASR_OCR_H
#define ASR_OCR_H

#include <AutoStarRail/IAsrBase.h>

struct AsrRect
{
    int32_t x;
    int32_t y;
    int32_t width;
    int32_t height;
};

ASR_INTERFACE IAsrOcrResult : public IAsrBase{};

ASR_INTERFACE IAsrOcrResults : public IAsrBase
{
    ASR_METHOD EnumResult(const size_t index, IAsrOcrResult** pp_out_result);
};

ASR_INTERFACE IAsrOcr : public IAsrBase
{
    ASR_METHOD FindText(IAsrOcrResult * *result, size_t * result_count);
};

#endif // ASR_OCR_H