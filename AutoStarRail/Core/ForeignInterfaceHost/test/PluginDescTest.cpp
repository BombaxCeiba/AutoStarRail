#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include "../src/ForeignInterfaceHost.h"
#include "../include/AutoStarRail/Core/ForeignInterfaceHost/AsrGuid.h"
#include <regex>
#include <cstring>

namespace
{
    template <class T>
    T JsonToStruct(const std::string& string)
    {
        const auto json = nlohmann::json::parse(string);
        T          result = json.template get<T>();
        return result;
    }
}

TEST(PluginDescTest, FromBasicJson)
{
    constexpr auto test_string = R"(
    {
        "name": "test_name",
        "author": "test_author",
        "version": "test_version",
        "guid" : "35BF38D4-7760-42EA-8A9C-9F2BF7C3CBDA",
        "description": "test_description",
        "settings": []
    }
    )";

    const auto plugin_desc =
        JsonToStruct<ASR::Core::ForeignInterfaceHost::PluginDesc>(test_string);

    EXPECT_EQ(plugin_desc.name, "test_name");
    EXPECT_EQ(plugin_desc.author, "test_author");
    EXPECT_EQ(plugin_desc.version, "test_version");
    const auto guid = ASR::Core::ForeignInterfaceHost::MakeAsrGuid(
        "35BF38D4-7760-42EA-8A9C-9F2BF7C3CBDA");
    EXPECT_EQ(plugin_desc.guid, guid);
}

TEST(PluginDescTest, FromUnexpectedGuidJson)
{
    constexpr auto test_string = R"(
{
        "name": "test_name",
        "author": "test_author",
        "version": "test_version",
        "guid" : "ufuoiajoighoa",
        "description": "test_description",
        "settings": []
}
    )";

    EXPECT_THROW(
        {
            const auto plugin_desc =
                JsonToStruct<ASR::Core::ForeignInterfaceHost::PluginDesc>(
                    test_string);
        },
        ASR::Core::InvalidGuidStringSizeException);
}

TEST(PluginSettingsDescTest, FromBasicJson)
{
    constexpr auto test_string = R"(
    {
        "name": "test_name",
        "author": "test_author",
        "version": "test_version",
        "guid" : "35BF38D4-7760-42EA-8A9C-9F2BF7C3CBDA",
        "description": "test_description",
        "settings": [{
            "name": "test_setting_name",
            "type": "Int",
            "defaultValue": 1,
            "description": "test_setting_description"
        }]
    }
    )";

    auto plugin_desc =
        JsonToStruct<ASR::Core::ForeignInterfaceHost::PluginDesc>(test_string);
    EXPECT_EQ(plugin_desc.name, "test_name");
    EXPECT_EQ(plugin_desc.author, "test_author");
    EXPECT_EQ(plugin_desc.version, "test_version");
    const auto guid = ASR::Core::ForeignInterfaceHost::MakeAsrGuid(
        "35BF38D4-7760-42EA-8A9C-9F2BF7C3CBDA");
    EXPECT_EQ(plugin_desc.guid, guid);

    const auto& setting_desc = plugin_desc.settings[0];

    EXPECT_EQ(setting_desc.name, "test_setting_name");
    EXPECT_EQ(setting_desc.type, AsrType::Int);
    EXPECT_EQ(std::get<std::int64_t>(setting_desc.default_value), 1);
    EXPECT_EQ(setting_desc.description, "test_setting_description");
}
