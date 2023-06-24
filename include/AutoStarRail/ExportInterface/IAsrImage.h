#ifndef ASR_IAMGE_H
#define ASR_IAMGE_H

#include <AutoStarRail/IAsrBase.h>

ASR_INTERFACE IAsrImage : public IAsrBase
{
    ASR_METHOD GetWidth(int* p_width);
    ASR_METHOD GetHeight(int* p_height);
    ASR_METHOD Clip(IAsrImage** p_out_image);
};

#endif // ASR_IAMGE_H
