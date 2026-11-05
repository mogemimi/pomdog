// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/network/host_port.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <ostream>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::network::detail::splitHostPort;

TEST_CASE("splitHostPort splits a host name and a port")
{
    const auto [host, port] = splitHostPort("example.com:80");
    REQUIRE(host == "example.com");
    REQUIRE(port == "80");
}

TEST_CASE("splitHostPort splits an IPv4 address and a port")
{
    const auto [host, port] = splitHostPort("127.0.0.1:30088");
    REQUIRE(host == "127.0.0.1");
    REQUIRE(port == "30088");
}

TEST_CASE("splitHostPort removes the brackets of an IPv6 address")
{
    const auto [host, port] = splitHostPort("[::1]:8080");
    REQUIRE(host == "::1");
    REQUIRE(port == "8080");
}

TEST_CASE("splitHostPort returns an empty port without a separator")
{
    const auto [host, port] = splitHostPort("localhost");
    REQUIRE(host == "localhost");
    REQUIRE(port.empty());
}

TEST_CASE("splitHostPort returns an empty host before the separator")
{
    const auto [host, port] = splitHostPort(":80");
    REQUIRE(host.empty());
    REQUIRE(port == "80");
}
