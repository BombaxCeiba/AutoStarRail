#include <AutoStarRail/Core/Exceptions/InvalidGuidStringException.h>
#include <format>

ASR_CORE_EXCEPTIONS_NS_BEGIN

InvalidGuidStringException::InvalidGuidStringException(
    const std::string_view invalid_guid_string)
    : Base{std::format(
        "Invalid GUID string. Current value is {}.",
        invalid_guid_string)}
{
}

InvalidGuidStringSizeException::InvalidGuidStringSizeException(
    const std::size_t string_size)
    : Base{std::format(
        "Size of AsrGuid string is not 36. Current value is {}.",
        string_size)}
{
}

ASR_CORE_EXCEPTIONS_NS_END