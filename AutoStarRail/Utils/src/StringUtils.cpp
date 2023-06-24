#include <AutoStarRail/Utils/StringUtils.h>
#include <algorithm>

ASR_UTILS_NS_BEGIN

void ToLowerInPlace(std::string& in_out_str)
{
    std::transform(
        in_out_str.begin(),
        in_out_str.end(),
        in_out_str.begin(),
        [](unsigned char c) { return std::tolower(c); });
}

void ToLowerInPlace(std::vector<std::string>& in_out_str_vector)
{
    for (auto& str : in_out_str_vector)
    {
        ToLowerInPlace(str);
    }
}

ASR_UTILS_NS_END
