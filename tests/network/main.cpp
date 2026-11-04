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
std::string_view httpsServerAddress;
std::string_view tcpServerAddress;
std::string_view tlsServerAddress;
std::string_view untrustedTLSServerAddress;
std::string_view udpServerAddress;
std::string_view caFile;

struct ServerOption final {
    std::string_view name;

    /// Describes the value in error messages, such as "<host:port>".
    std::string_view placeholder;

    std::string_view* value = nullptr;
};

} // namespace

std::string_view getHTTPServerAddress() noexcept
{
    return httpServerAddress;
}

std::string_view getHTTPSServerAddress() noexcept
{
    return httpsServerAddress;
}

std::string_view getTCPServerAddress() noexcept
{
    return tcpServerAddress;
}

std::string_view getTLSServerAddress() noexcept
{
    return tlsServerAddress;
}

std::string_view getUntrustedTLSServerAddress() noexcept
{
    return untrustedTLSServerAddress;
}

std::string_view getUDPServerAddress() noexcept
{
    return udpServerAddress;
}

std::string_view getCAFile() noexcept
{
    return caFile;
}

} // namespace pomdog::testing

int main(int argc, char** argv)
{
    using pomdog::testing::ServerOption;

    constexpr std::string_view address = "<host:port>";
    const std::array<ServerOption, 7> serverOptions = {{
        {.name = "--http-server", .placeholder = address, .value = &pomdog::testing::httpServerAddress},
        {.name = "--https-server", .placeholder = address, .value = &pomdog::testing::httpsServerAddress},
        {.name = "--tcp-server", .placeholder = address, .value = &pomdog::testing::tcpServerAddress},
        {.name = "--tls-server", .placeholder = address, .value = &pomdog::testing::tlsServerAddress},
        {.name = "--untrusted-tls-server", .placeholder = address, .value = &pomdog::testing::untrustedTLSServerAddress},
        {.name = "--udp-server", .placeholder = address, .value = &pomdog::testing::udpServerAddress},
        {.name = "--ca-file", .placeholder = "<path>", .value = &pomdog::testing::caFile},
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
                std::cerr << "error: write " << option.name << "=" << option.placeholder << " with an equals sign\n";
                return 1;
            }
            if (view.starts_with(option.name) && (view.substr(option.name.size()).starts_with('='))) {
                *option.value = view.substr(option.name.size() + 1);
                isServerOption = true;
                break;
            }
        }
        if (!isServerOption) {
            doctestArgs.push_back(arg);
        }
    }

    for (const auto& option : serverOptions) {
        if (option.value->empty()) {
            std::cerr << "error: " << option.name << "=" << option.placeholder << " is missing. "
                      << "Run the tests with tools/cmd/network-test-runner, which starts the servers.\n";
            return 1;
        }
    }

    doctest::Context context;
    context.applyCommandLine(static_cast<int>(doctestArgs.size()), doctestArgs.data());
    return context.run();
}
