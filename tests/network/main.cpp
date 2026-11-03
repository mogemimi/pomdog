// Copyright mogemimi. Distributed under the MIT license.

#define DOCTEST_CONFIG_IMPLEMENT

#include "tests/network/network_test_options.h"
#include "tests/testing/testing.h"
#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <array>
#include <cstddef>
#include <iostream>
#include <span>
#include <string_view>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

namespace pomdog::testing {
namespace {

std::string_view httpServerAddress;
std::string_view tcpServerAddress;
std::string_view udpServerAddress;

struct ServerOption final {
    std::string_view name;
    std::string_view* address = nullptr;
};

} // namespace

std::string_view getHTTPServerAddress() noexcept
{
    return httpServerAddress;
}

std::string_view getTCPServerAddress() noexcept
{
    return tcpServerAddress;
}

std::string_view getUDPServerAddress() noexcept
{
    return udpServerAddress;
}

} // namespace pomdog::testing

int main(int argc, char** argv)
{
    using pomdog::testing::ServerOption;

    const std::array<ServerOption, 3> serverOptions = {{
        {.name = "--http-server", .address = &pomdog::testing::httpServerAddress},
        {.name = "--tcp-server", .address = &pomdog::testing::tcpServerAddress},
        {.name = "--udp-server", .address = &pomdog::testing::udpServerAddress},
    }};

    POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
    const auto args = std::span<char*>{argv, static_cast<std::size_t>(argc)};
    POMDOG_CLANG_UNSAFE_BUFFER_END

    // NOTE: Take the server addresses, and pass the rest to doctest.
    std::vector<const char*> doctestArgs;
    doctestArgs.reserve(args.size());

    for (const char* arg : args) {
        const auto view = std::string_view{arg};
        bool isServerOption = false;
        for (const auto& option : serverOptions) {
            if (view == option.name) {
                std::cerr << "error: write " << option.name << "=<host:port> with an equals sign\n";
                return 1;
            }
            if (view.starts_with(option.name) && (view.substr(option.name.size()).starts_with('='))) {
                *option.address = view.substr(option.name.size() + 1);
                isServerOption = true;
                break;
            }
        }
        if (!isServerOption) {
            doctestArgs.push_back(arg);
        }
    }

    for (const auto& option : serverOptions) {
        if (option.address->empty()) {
            std::cerr << "error: " << option.name << "=<host:port> is missing. "
                      << "Run the tests with tools/cmd/network-test-runner, which starts the servers.\n";
            return 1;
        }
    }

    doctest::Context context;
    context.applyCommandLine(static_cast<int>(doctestArgs.size()), doctestArgs.data());
    return context.run();
}
