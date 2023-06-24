#ifndef ASR_CORE_EXCEPTIONS_PYTHONEXCEPTION_H
#define ASR_CORE_EXCEPTIONS_PYTHONEXCEPTION_H

#include <AutoStarRail/Core/Exceptions/Config.h>
#include <AutoStarRail/IAsrBase.h>

ASR_CORE_EXCEPTIONS_NS_BEGIN

class PythonException : public std::runtime_error
{
    using Base = std::runtime_error;

public:
    ASR_USING_BASE_CTOR(Base);
};

ASR_CORE_EXCEPTIONS_NS_END

#endif // ASR_CORE_EXCEPTIONS_PYTHONEXCEPTION_H
