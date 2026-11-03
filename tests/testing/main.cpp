// Copyright mogemimi. Distributed under the MIT license.

#define DOCTEST_CONFIG_IMPLEMENT

#include "tests/testing/test_options.h"
#include "tests/testing/testing.h"
#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <cstddef>
#include <iostream>
#include <span>
#include <string_view>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

namespace pomdog::testing {
namespace {

std::string_view pomdogDir;

} // namespace

std::string_view getPomdogDir() noexcept
{
    return pomdogDir;
}

} // namespace pomdog::testing

int main(int argc, char** argv)
{
    constexpr std::string_view pomdogDirOption = "--pomdog-dir";
    constexpr std::string_view pomdogDirPrefix = "--pomdog-dir=";

    POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
    const auto args = std::span<char*>{argv, static_cast<std::size_t>(argc)};
    POMDOG_CLANG_UNSAFE_BUFFER_END

    // NOTE: Take the options of the Pomdog tests, and pass the rest to doctest.
    std::vector<const char*> doctestArgs;
    doctestArgs.reserve(args.size());

    for (const char* arg : args) {
        const auto view = std::string_view{arg};
        if (view.starts_with(pomdogDirPrefix)) {
            pomdog::testing::pomdogDir = view.substr(pomdogDirPrefix.size());
            if (pomdog::testing::pomdogDir.empty()) {
                std::cerr << "error: --pomdog-dir needs a path, such as --pomdog-dir=path/to/pomdog\n";
                return 1;
            }
            continue;
        }
        if (view == pomdogDirOption) {
            std::cerr << "error: write --pomdog-dir=<path> with an equals sign\n";
            return 1;
        }
        doctestArgs.push_back(arg);
    }

    doctest::Context context;
    context.applyCommandLine(static_cast<int>(doctestArgs.size()), doctestArgs.data());

    if (pomdog::testing::pomdogDir.empty()) {
        // NOTE: The tests that read files from the checkout need its path.
        context.addFilter("test-suite-exclude", pomdog::testing::pomdogDirTestSuite);
    }

    return context.run();
}
