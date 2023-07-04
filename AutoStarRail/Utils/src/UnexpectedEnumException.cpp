#include <AutoStarRail/Utils/UnexpectedEnumException.h>
#include <AutoStarRail/Utils/fmt.h>

ASR_UTILS_NS_BEGIN

ASR_NS_ANONYMOUS_DETAILS_BEGIN

ASR_NS_ANONYMOUS_DETAILS_END

UnexpectedEnumException::UnexpectedEnumException(
    const std::string_view u8_enum_value)
    : Base{ASR::fmt::format("Unexpected enum found. Value = {}", u8_enum_value)}
{
}

UnexpectedEnumException::UnexpectedEnumException(
    std::int64_t enum_value)
    : Base{ASR::fmt::format("Unexpected enum found. Value = {}", enum_value)}
{
}

UnexpectedEnumException::UnexpectedEnumException(std::uint64_t enum_value)
    : Base{ASR::fmt::format("Unexpected enum found. Value = {}", enum_value)}
{
}

ASR_UTILS_NS_END
