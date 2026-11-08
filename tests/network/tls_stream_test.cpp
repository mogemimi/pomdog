// Copyright mogemimi. Distributed under the MIT license.

#include "distrusted_keys_helper.h"
#include "network_test_helper.h"
#include "network_test_options.h"
#include "tests/testing/testing.h"
#include "pomdog/basic/types.h"
#include "pomdog/network/stream.h"
#include "pomdog/network/tls_stream.h"
#include "pomdog/network/trust_store.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <array>
#include <chrono>
#include <memory>
#include <ostream>
#include <span>
#include <string>
#include <string_view>
#include <thread>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

// NOTE: Suppress C4866 warning for doctest expressions in this file
POMDOG_MSVC_SUPPRESS_WARNING(4866)

using namespace pomdog;
using namespace pomdog::network;
using pomdog::testing::getCAKeyHash;
using pomdog::testing::getRSAHTTPSServerAddress;
using pomdog::testing::getServerKeyHash;
using pomdog::testing::getTLS12HTTPSServerAddress;
using pomdog::testing::getTLSServerAddress;
using pomdog::testing::getUntrustedTLSServerAddress;
using pomdog::testing::KeyHash;
using pomdog::testing::makeDistrustedKeysFile;
using pomdog::testing::parseKeyHash;

// NOTE: The TLS servers echo each line, and close the connection after they
// echo "bye\n", or without close_notify after "drop\n". Their certificates
// name only the IP address 127.0.0.1.

namespace {

/// Starts a TLS connection to address that trusts the root of the local
/// servers, and distrusts distrustedKeys, a `.dkeys` file or empty.
[[nodiscard]] std::shared_ptr<Stream>
connectToLocalServer(std::string_view address, std::span<const u8> distrustedKeys = {})
{
    auto result = connectTLSStream(address, loadTestTrustStore(distrustedKeys), StreamOptions{});
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

TEST_CASE("TLS stream reads the bytes that arrived during a pause longer than the idle timeout")
{
    StreamOptions options;
    options.idleTimeout = std::chrono::milliseconds{200};

    auto result = connectTLSStream(getTLSServerAddress(), loadTestTrustStore(), options);
    REQUIRE(result.has_value());
    auto stream = std::move(*result);
    waitForConnected(*stream);
    REQUIRE(stream->write(asBytes("hello\n")).has_value());

    // NOTE: The echo arrives while the game calls no update(), such as during
    // a long load.
    std::this_thread::sleep_for(std::chrono::milliseconds{400});
    REQUIRE(receiveText(*stream, 6) == "hello\n");
    REQUIRE(stream->isConnected());
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

// NOTE: The TLS 1.2 and RSA servers speak HTTP, so these tests check only the
// handshake.
TEST_CASE("TLS stream connects to a TLS 1.2 server")
{
    auto stream = connectToLocalServer(getTLS12HTTPSServerAddress());
    waitForConnected(*stream);
    stream->close();
}

TEST_CASE("TLS stream connects to a server with an RSA certificate chain")
{
    auto stream = connectToLocalServer(getRSAHTTPSServerAddress());
    waitForConnected(*stream);
    stream->close();
}

TEST_CASE("TLS stream rejects a server that the trust store does not verify")
{
    auto stream = connectToLocalServer(getUntrustedTLSServerAddress());

    const auto err = waitForDisconnected(*stream);
    REQUIRE_MESSAGE(isVerificationError(err), describe(err));
}

TEST_CASE("TLS stream rejects a server whose key is distrusted")
{
    const std::array<KeyHash, 1> hashes = {parseKeyHash(getServerKeyHash())};
    const auto keys = makeDistrustedKeysFile(hashes);
    auto stream = connectToLocalServer(getTLSServerAddress(), keys);

    const auto err = waitForDisconnected(*stream);
    REQUIRE(err != nullptr);
    REQUIRE_MESSAGE(err->toString().find("distrusted key") != std::string::npos, describe(err));
}

TEST_CASE("TLS stream connects to a server whose keys are not distrusted")
{
    KeyHash first = {};
    KeyHash second = {};
    first.fill(0x00);
    second.fill(0xff);
    const std::array<KeyHash, 2> hashes = {first, second};
    const auto keys = makeDistrustedKeysFile(hashes);
    auto stream = connectToLocalServer(getTLSServerAddress(), keys);
    waitForConnected(*stream);

    REQUIRE(stream->write(asBytes("hello\n")).has_value());
    REQUIRE(receiveText(*stream, 6) == "hello\n");
    stream->close();
}

TEST_CASE("createTrustStore fails when the root of the local servers has a distrusted key")
{
    const auto pem = readTestRootCertificate();
    const std::array<KeyHash, 1> hashes = {parseKeyHash(getCAKeyHash())};
    const auto keys = makeDistrustedKeysFile(hashes);

    TrustStoreDesc desc;
    desc.rootCertificatesPEM = asBytes(pem);
    desc.distrustedKeys = keys;
    auto store = createTrustStore(desc);
    REQUIRE_FALSE(store.has_value());
    REQUIRE(store.error() != nullptr);
    REQUIRE_MESSAGE(store.error()->toString().find("distrusted key") != std::string::npos, describe(store.error()));
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

TEST_CASE("TLS stream reports a close without close_notify as Disconnected with an error")
{
    auto stream = connectToLocalServer(getTLSServerAddress());
    waitForConnected(*stream);

    // NOTE: After "drop\n", the server closes the TCP connection without
    // close_notify, which could cut the data short.
    REQUIRE(stream->write(asBytes("drop\n")).has_value());
    REQUIRE(receiveText(*stream, 5) == "drop\n");

    const auto err = waitForDisconnected(*stream);
    REQUIRE(err != nullptr);
    REQUIRE_MESSAGE(err->toString().find("close_notify") != std::string::npos, describe(err));
}
