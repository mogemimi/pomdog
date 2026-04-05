// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/application/win32/dark_mode.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::detail::win32::isWindowsVersionOrGreaterForWindows10;

TEST_CASE("DarkMode")
{
    SUBCASE("isWindowsVersionOrGreaterForWindows10_Windows10")
    {
        // NOTE: We are running on Windows 10+, so version 10.0.0 should always be true.
        CHECK(isWindowsVersionOrGreaterForWindows10(10, 0, 0));
    }
    SUBCASE("isWindowsVersionOrGreaterForWindows10_LowerVersion")
    {
        // NOTE: A future major version should not be satisfied.
        CHECK_FALSE(isWindowsVersionOrGreaterForWindows10(99, 0, 0));
    }
    SUBCASE("isWindowsVersionOrGreaterForWindows10_BuildNumber17763")
    {
        // NOTE: Build 17763 (Windows 10 1809) is the minimum for dark mode.
        // On any modern CI the build number should be >= 17763.
        CHECK(isWindowsVersionOrGreaterForWindows10(10, 0, 17763));
    }
    SUBCASE("isWindowsVersionOrGreaterForWindows10_VeryHighBuild")
    {
        // NOTE: An unrealistically high build number should not be satisfied.
        CHECK_FALSE(isWindowsVersionOrGreaterForWindows10(10, 0, 65535));
    }
}
