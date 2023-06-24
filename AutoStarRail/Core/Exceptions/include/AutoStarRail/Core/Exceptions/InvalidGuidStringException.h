#ifndef ASR_CORE_EXCEPTIONS_INVALIDGUIDSTRINGEXCEPTION_H
#define ASR_CORE_EXCEPTIONS_INVALIDGUIDSTRINGEXCEPTION_H

#include <AutoStarRail/Core/Exceptions/Config.h>
#include <string_view>

ASR_CORE_EXCEPTIONS_NS_BEGIN

struct InvalidGuidStringException : public std::runtime_error
{
    using Base = std::runtime_error;
    explicit InvalidGuidStringException(const std::string_view invalid_guid_string);
};

struct InvalidGuidStringSizeException : public std::runtime_error
{
    using Base = std::runtime_error;
    explicit InvalidGuidStringSizeException(const std::size_t string_size);
};

ASR_CORE_EXCEPTIONS_NS_END

#endif // ASR_CORE_EXCEPTIONS_INVALIDGUIDSTRINGEXCEPTION_H
