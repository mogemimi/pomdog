// Copyright mogemimi. Distributed under the MIT license.

#define DOCTEST_CONFIG_IMPLEMENT

#include "tests/network/network_test_options.h"
#include "tests/testing/testing.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/platform.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#if defined(POMDOG_PLATFORM_EMSCRIPTEN)
#include <emscripten.h>
#endif
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
std::string_view tls12HTTPSServerAddress;
std::string_view rsaHTTPSServerAddress;
std::string_view tcpServerAddress;
std::string_view tlsServerAddress;
std::string_view untrustedTLSServerAddress;
std::string_view udpServerAddress;
std::string_view closedTCPServerAddress;
std::string_view caFile;
std::string_view caKeyHash;
std::string_view serverKeyHash;

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

std::string_view getTLS12HTTPSServerAddress() noexcept
{
    return tls12HTTPSServerAddress;
}

std::string_view getRSAHTTPSServerAddress() noexcept
{
    return rsaHTTPSServerAddress;
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

std::string_view getClosedTCPServerAddress() noexcept
{
    return closedTCPServerAddress;
}

std::string_view getCAFile() noexcept
{
    return caFile;
}

std::string_view getCAKeyHash() noexcept
{
    return caKeyHash;
}

std::string_view getServerKeyHash() noexcept
{
    return serverKeyHash;
}

} // namespace pomdog::testing

int main(int argc, char** argv)
{
    using pomdog::testing::ServerOption;

    constexpr std::string_view address = "<host:port>";
    constexpr std::string_view hash = "<hex>";
    const std::array<ServerOption, 12> serverOptions = {{
        {.name = "--http-server", .placeholder = address, .value = &pomdog::testing::httpServerAddress},
        {.name = "--https-server", .placeholder = address, .value = &pomdog::testing::httpsServerAddress},
        {.name = "--tls12-https-server", .placeholder = address, .value = &pomdog::testing::tls12HTTPSServerAddress},
        {.name = "--rsa-https-server", .placeholder = address, .value = &pomdog::testing::rsaHTTPSServerAddress},
        {.name = "--tcp-server", .placeholder = address, .value = &pomdog::testing::tcpServerAddress},
        {.name = "--tls-server", .placeholder = address, .value = &pomdog::testing::tlsServerAddress},
        {.name = "--untrusted-tls-server", .placeholder = address, .value = &pomdog::testing::untrustedTLSServerAddress},
        {.name = "--udp-server", .placeholder = address, .value = &pomdog::testing::udpServerAddress},
        {.name = "--closed-tcp-server", .placeholder = address, .value = &pomdog::testing::closedTCPServerAddress},
        {.name = "--ca-file", .placeholder = "<path>", .value = &pomdog::testing::caFile},
        {.name = "--ca-key-hash", .placeholder = hash, .value = &pomdog::testing::caKeyHash},
        {.name = "--server-key-hash", .placeholder = hash, .value = &pomdog::testing::serverKeyHash},
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
    const int result = context.run();

#if defined(POMDOG_PLATFORM_EMSCRIPTEN)
    // NOTE: The tests return to the event loop while they wait, and then the
    // runtime ignores the value that main() returns.
    emscripten_force_exit(result);
#else
    return result;
#endif
}
