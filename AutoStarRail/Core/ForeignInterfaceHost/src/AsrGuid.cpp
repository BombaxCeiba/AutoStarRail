#include "AutoStarRail/AsrString.hpp"
#include <AutoStarRail/Core/ForeignInterfaceHost/AsrGuid.h>
#include <cstdio>
#include <array>
#include <cstring>
#include <nlohmann/json.hpp>

auto std::formatter<AsrGuid, char>::format(
    const AsrGuid&  guid,
    format_context& ctx) const ->
    typename std::remove_reference_t<decltype(ctx)>::iterator
{
    constexpr auto template_string =
        "{{{:08x}-{:04x}-{:04x}-{:02x}{:02x}-{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}}}";

    return formatter<string>::format(
        std::format(
            template_string,
            guid.data1,
            guid.data2,
            guid.data3,
            guid.data4[0],
            guid.data4[1],
            guid.data4[2],
            guid.data4[3],
            guid.data4[4],
            guid.data4[5],
            guid.data4[6],
            guid.data4[7]),
        ctx);
}

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

AsrGuid MakeAsrGuid(const std::string_view guid_string)
{
    AsrGuid       result;
    unsigned long p0;
    static_assert(sizeof(p0) == sizeof(result.data1), "p0 type not match.");
    unsigned short p1, p2;
    static_assert(sizeof(p1) == sizeof(result.data2), "p1 type not match.");
    unsigned char p3, p4, p5, p6, p7, p8, p9, p10;
    static_assert(sizeof(p3) == sizeof(result.data4[0]), "p3 type not match.");
    std::array<char, 37> tmp_buffer;

    // 检查GUID字符串长度
    if (const auto string_size = guid_string.size(); //
        string_size != 36)
    {
        throw InvalidGuidStringSizeException(string_size);
    }

    std::memcpy(tmp_buffer.data(), guid_string.data(), 36);
    tmp_buffer[36] = '\0';

    const auto err = std::sscanf(
        guid_string.data(),
        "%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX",
        &p0,
        &p1,
        &p2,
        &p3,
        &p4,
        &p5,
        &p6,
        &p7,
        &p8,
        &p9,
        &p10);
    if (err != 11)
    {
        throw InvalidGuidStringException(guid_string);
    }

    result.data1 = p0;
    result.data2 = p1;
    result.data3 = p2;
    result.data4[0] = p3;
    result.data4[1] = p4;
    result.data4[2] = p5;
    result.data4[3] = p6;
    result.data4[4] = p7;
    result.data4[5] = p8;
    result.data4[6] = p9;
    result.data4[7] = p10;

    return result;
}

void from_json(const nlohmann::json& input, AsrGuid& output)
{
    const auto guid_string = input.get<std::string_view>();
    output = MakeAsrGuid(guid_string);
}

ASR_CORE_FOREIGNINTERFACEHOST_NS_END

AsrString AsrGuidToString(const AsrGuid& guid)
{
    constexpr auto template_string =
        "{:08x}-{:04x}-{:04x}-{:02x}{:02x}-{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}";
    const auto string = std::format(
        template_string,
        guid.data1,
        guid.data2,
        guid.data3,
        guid.data4[0],
        guid.data4[1],
        guid.data4[2],
        guid.data4[3],
        guid.data4[4],
        guid.data4[5],
        guid.data4[6],
        guid.data4[7]);

    ASR::AsrPtr<IAsrString> p_result;
    ::CreateIAsrStringFromUtf8(string.c_str(), p_result.Put());
    return AsrString{p_result};
}