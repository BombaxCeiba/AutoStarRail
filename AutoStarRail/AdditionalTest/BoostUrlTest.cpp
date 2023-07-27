#include "boost/url/urls.hpp"
#include <boost/url.hpp>
#include <gtest/gtest.h>
#include <iostream>

TEST(BoostImplementationCheck, UrlExceptionTest)
{
    // Will not throw exceptions.
    boost::url_view test{""};
    // Use empty value to indicate failure.
    EXPECT_EQ(test.scheme(), std::string_view{});

    test = {"adb"};
    EXPECT_EQ(test.scheme(), std::string_view{});

    test = {"adb://"};
    EXPECT_EQ(test.scheme(), std::string_view{"adb"});
}

TEST(BoostImplementationCheck, UrlPartlyResolvedTest)
{
    boost::url test{"192.168.1.1:12345"};
    EXPECT_EQ(test.host_ipv4_address().to_string(), "192.168.1.1");
    EXPECT_EQ(test.port_number(), 12345);
}
