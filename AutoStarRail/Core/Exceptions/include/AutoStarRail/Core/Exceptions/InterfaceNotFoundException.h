#ifndef ASR_CORE_EXCEPTIONS_INTERFACENOTFOUNDEXCEPTION_H
#define ASR_CORE_EXCEPTIONS_INTERFACENOTFOUNDEXCEPTION_H

#include <AutoStarRail/Core/Exceptions/Config.h>
#include <AutoStarRail/IAsrBase.h>

ASR_CORE_EXCEPTIONS_NS_BEGIN

class InterfaceNotFoundException : public std::runtime_error
{
    using Base = std::runtime_error;

public:
    explicit InterfaceNotFoundException(const AsrGuid& iid);
};

ASR_CORE_EXCEPTIONS_NS_END

#endif // ASR_CORE_EXCEPTIONS_INTERFACENOTFOUNDEXCEPTION_H