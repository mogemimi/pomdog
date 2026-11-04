// Copyright mogemimi. Distributed under the MIT license.

#include "network_test_helper.h"
#include "network_test_options.h"
#include "tests/testing/testing.h"
#include "pomdog/network/stream.h"
#include "pomdog/network/tls_stream.h"
#include "pomdog/network/trust_store.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <memory>
#include <ostream>
#include <string>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

// NOTE: Suppress C4866 warning for doctest expressions in this file
POMDOG_MSVC_SUPPRESS_WARNING(4866)

using namespace pomdog;
using namespace pomdog::network;
using pomdog::testing::getTLSServerAddress;
using pomdog::testing::getUntrustedTLSServerAddress;

// NOTE: The TLS servers echo each line, and close the connection after they
// echo "bye\n". Their certificates name only the IP address 127.0.0.1.

namespace {

/// Starts a TLS connection to address that trusts the root of the local
/// servers.
[[nodiscard]] std::shared_ptr<Stream>
connectToLocalServer(std::string_view address)
{
    auto result = connectTLSStream(address, loadTestTrustStore(), StreamOptions{});
    if (!result) {
        FAIL("failed to start connecting: " << describe(result.error()));
    }
    REQUIRE(*result != nullptr);
    return std::move(*result);
}

/// Returns true if err says that the server certificate failed verification.
[[nodiscard]] bool
isVerificationError(const std::unique_ptr<Error>& err)
{
    return (err != nullptr) && (err->toString().find("failed verification") != std::string::npos);
}

} // namespace

TEST_CASE("TLS stream echoes lines through the local server")
{
    auto stream = connectToLocalServer(getTLSServerAddress());
    REQUIRE_FALSE(stream->isConnected());
    waitForConnected(*stream);

    REQUIRE(stream->write(asBytes("hello\n")).has_value());
    REQUIRE(stream->write(asBytes("world\n")).has_value());
    REQUIRE(receiveText(*stream, 12) == "hello\nworld\n");

    stream->close();
    REQUIRE_FALSE(stream->isConnected());

    // NOTE: A closed stream reports no events and cannot send.
    stream->update();
    REQUIRE_FALSE(stream->pollEvent().has_value());
    REQUIRE_FALSE(stream->write(asBytes("hello\n")).has_value());
}

TEST_CASE("TLS stream sends a write that is larger than a TLS record")
{
    auto stream = connectToLocalServer(getTLSServerAddress());
    waitForConnected(*stream);

    // NOTE: A TLS record holds up to 16 KiB, so the stream sends this line in
    // several records.
    const auto line = makeLetters(40000) + "\n";
    REQUIRE(stream->write(asBytes(line)).has_value());
    REQUIRE(receiveText(*stream, line.size()) == line);
}

TEST_CASE("TLS stream reports Disconnected without an error when the server closes the connection")
{
    auto stream = connectToLocalServer(getTLSServerAddress());
    waitForConnected(*stream);

    REQUIRE(stream->write(asBytes("bye\n")).has_value());
    REQUIRE(receiveText(*stream, 4) == "bye\n");

    // NOTE: The server sends close_notify before it closes the connection.
    const auto err = waitForDisconnected(*stream);
    REQUIRE_MESSAGE(err == nullptr, describe(err));

    stream->update();
    REQUIRE_FALSE(stream->pollEvent().has_value());
}

TEST_CASE("TLS stream reports a failed send as Disconnected with an error")
{
    auto stream = connectToLocalServer(getTLSServerAddress());
    waitForConnected(*stream);

    REQUIRE(stream->write(asBytes("bye\n")).has_value());

    // NOTE: The stream is not updated, so it does not read the answer or that
    // the server closed the connection. One update can read both.
    writeUntilSendFails(*stream);
}

TEST_CASE("TLS stream rejects a server that the trust store does not verify")
{
    auto stream = connectToLocalServer(getUntrustedTLSServerAddress());

    const auto err = waitForDisconnected(*stream);
    REQUIRE_MESSAGE(isVerificationError(err), describe(err));
}

TEST_CASE("TLS stream rejects a certificate that names another host")
{
    // NOTE: The certificate names 127.0.0.1, which "localhost" does not match.
    const auto address = getTLSServerAddress();
    const auto port = address.substr(address.rfind(':') + 1);
    auto stream = connectToLocalServer("localhost:" + std::string{port});

    const auto err = waitForDisconnected(*stream);
    REQUIRE_MESSAGE(isVerificationError(err), describe(err));
}

TEST_CASE("connectTLSStream fails without a trust store")
{
    auto result = connectTLSStream(getTLSServerAddress(), nullptr, StreamOptions{});
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error() != nullptr);
}

TEST_CASE("TLS stream that is closed while connecting reports no events")
{
    auto stream = connectToLocalServer(getTLSServerAddress());

    stream->close();
    REQUIRE_FALSE(stream->isConnected());

    for (int i = 0; i < 10; ++i) {
        stream->update();
        REQUIRE_FALSE(stream->pollEvent().has_value());
    }
    REQUIRE_FALSE(stream->write(asBytes("hello\n")).has_value());
}
