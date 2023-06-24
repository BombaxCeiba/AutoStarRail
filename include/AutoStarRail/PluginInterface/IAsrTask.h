#ifndef ASR_ITASK_H
#define ASR_ITASK_H

#include <AutoStarRail/IAsrBase.h>

ASR_DEFINE_GUID(
    ASR_IID_TASK,
    IAsrTask,
    0xace44315,
    0x637c,
    0x4e83,
    0xab,
    0xe0,
    0xda,
    0xa9,
    0xa,
    0x82,
    0xa6,
    0xca)
ASR_INTERFACE IAsrTask : public IAsrBase{};

#endif // ASR_ITASK_H
