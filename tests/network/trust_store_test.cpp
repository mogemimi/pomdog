// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/test_options.h"
#include "tests/testing/testing.h"
#include "pomdog/basic/platform.h"
#include "pomdog/basic/types.h"
#include "pomdog/network/trust_store.h"
#include "pomdog/utility/path_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <fstream>
#include <iterator>
#include <span>
#include <string>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

// NOTE: Suppress C4866 warning for doctest expressions in this file
POMDOG_MSVC_SUPPRESS_WARNING(4866)

using pomdog::u8;
using pomdog::network::createTrustStoreFromPEM;

namespace {

[[nodiscard]] std::span<const u8>
asBytes(std::string_view text) noexcept
{
    POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
    return std::span<const u8>{reinterpret_cast<const u8*>(text.data()), text.size()};
    POMDOG_CLANG_UNSAFE_BUFFER_END
}

/// Returns the root certificates that the engine assets provide.
[[nodiscard]] std::string
readRootCertificates()
{
    const auto path = pomdog::filepaths::join(pomdog::testing::getPomdogDir(), "assets/certificates/root_certificates.pem");
    std::ifstream file{path, std::ios::binary};
    REQUIRE_MESSAGE(file.is_open(), "failed to open " << path);
    return std::string{std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{}};
}

} // namespace

TEST_CASE("createTrustStoreFromPEM reads the engine root certificates" * doctest::test_suite(pomdog::testing::pomdogDirTestSuite))
{
    auto store = createTrustStoreFromPEM(asBytes(readRootCertificates()));
    REQUIRE(store.has_value());
    REQUIRE(*store != nullptr);
}

#if defined(POMDOG_PLATFORM_EMSCRIPTEN)
TEST_CASE("createTrustStoreFromPEM does not read the PEM on the web")
{
    auto store = createTrustStoreFromPEM(asBytes("no certificate"));
    REQUIRE(store.has_value());
    REQUIRE(*store != nullptr);
}
#else
TEST_CASE("createTrustStoreFromPEM fails without a certificate")
{
    REQUIRE_FALSE(createTrustStoreFromPEM(std::span<const u8>{}).has_value());
    REQUIRE_FALSE(createTrustStoreFromPEM(asBytes("no certificate")).has_value());
}

TEST_CASE("createTrustStoreFromPEM fails when a certificate cannot be parsed" * doctest::test_suite(pomdog::testing::pomdogDirTestSuite))
{
    // NOTE: The last block decodes to bytes that are not a certificate.
    const auto pem = readRootCertificates() + "-----BEGIN CERTIFICATE-----\nAAAA\n-----END CERTIFICATE-----\n";
    auto store = createTrustStoreFromPEM(asBytes(pem));
    REQUIRE_FALSE(store.has_value());
    REQUIRE(store.error() != nullptr);
}
#endif
