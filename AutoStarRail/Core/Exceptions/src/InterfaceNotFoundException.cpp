#include <AutoStarRail/Core/Exceptions/InterfaceNotFoundException.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/AsrGuid.h>

ASR_CORE_EXCEPTIONS_NS_BEGIN

InterfaceNotFoundException::InterfaceNotFoundException(const AsrGuid& iid)
    : Base{ASR::fmt::format(
        "Attempt to convert interface to one that not exist. Interface id = {} .",
        iid)}
{
}

ASR_CORE_EXCEPTIONS_NS_END
