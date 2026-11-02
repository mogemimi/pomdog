// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "tests/testing/testing.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <chrono>
#include <functional>
#include <string>
#include <thread>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

namespace pomdog::network {

/// Calls step about once per millisecond until it returns true, like a game
/// loop that updates network objects every frame. Returns false when that
/// takes longer than timeout, which is longer than the network timeouts that
/// the tests use.
[[nodiscard]] inline bool
runUntil(const std::function<bool()>& step, std::chrono::seconds timeout = std::chrono::seconds{30})
{
    const auto startTime = std::chrono::steady_clock::now();
    while (!step()) {
        if ((std::chrono::steady_clock::now() - startTime) > timeout) {
            return false;
        }

        // NOTE: I want to suppress energy impact if possible.
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    return true;
}

/// Reports err, which a test that needs the network gets when the host cannot
/// be reached; the test then ends without checking the response. A certificate
/// that the embedded root certificates do not verify fails the test instead,
/// because that is a defect rather than a network problem.
inline void
reportNetworkError(const Error& err)
{
    const auto message = err.toString();

    // NOTE: The TLS stream reports a certificate that fails verification so.
    CHECK_MESSAGE(message.find("mbedtls_ssl_get_verify_result") == std::string::npos, message);
    MESSAGE(message);
}

} // namespace pomdog::network
