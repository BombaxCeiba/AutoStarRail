#ifndef ASR_UTILS_STRINGUTILS_HPP
#define ASR_UTILS_STRINGUTILS_HPP

#include <AutoStarRail/Utils/Config.h>
#include <string>
#include <vector>

ASR_UTILS_NS_BEGIN

void ToLowerInPlace(std::string& in_out_str);

void ToLowerInPlace(std::vector<std::string>& in_out_str_vector);

ASR_UTILS_NS_END

#endif // ASR_UTILS_STRINGUTILS_HPP
