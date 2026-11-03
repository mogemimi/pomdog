// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "tests/testing/testing.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

namespace pomdog::testing {

/// Names the test suite of the tests that read files from the Pomdog checkout,
/// such as engine assets. The test runner skips this suite unless the command
/// line names the checkout with `--pomdog-dir=<path>`:
/// ```cpp
/// TEST_CASE("reads an asset" * doctest::test_suite(pomdog::testing::pomdogDirTestSuite))
/// {
///     const auto path = pomdog::filepaths::join(pomdog::testing::getPomdogDir(), "assets/...");
/// }
/// ```
inline constexpr const char* pomdogDirTestSuite = "pomdog-dir";

/// Returns the Pomdog checkout directory that `--pomdog-dir` named, or an
/// empty string when the command line named none.
[[nodiscard]] std::string_view
getPomdogDir() noexcept;

} // namespace pomdog::testing
