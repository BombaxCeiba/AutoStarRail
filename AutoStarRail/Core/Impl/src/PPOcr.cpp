#include <AutoStarRail/Core/Impl/AsrOrt.h>

ASR_CORE_IMPL_NS_BEGIN

class PPOcr : public AsrOrt
{
    using Base = AsrOrt;
    Ort::Session session_;

public:
    PPOcr();
};

ASR_CORE_IMPL_NS_END