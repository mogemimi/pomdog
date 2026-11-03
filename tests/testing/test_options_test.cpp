// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/test_options.h"
#include "tests/testing/testing.h"
#include "pomdog/filesystem/file_system.h"
#include "pomdog/utility/path_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

TEST_CASE("--pomdog-dir names the Pomdog checkout" * doctest::test_suite(pomdog::testing::pomdogDirTestSuite))
{
    const auto dir = pomdog::testing::getPomdogDir();
    REQUIRE_FALSE(dir.empty());

    // NOTE: The checkout holds the engine sources.
    const auto path = pomdog::filepaths::join(dir, "pomdog/basic/version.h");
    REQUIRE(pomdog::platformfs::exists(path));
}
