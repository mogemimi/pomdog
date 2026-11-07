// Copyright mogemimi. Distributed under the MIT license.

#include "distrusted_keys_helper.h"
#include "tests/testing/test_options.h"
#include "tests/testing/testing.h"
#include "pomdog/basic/platform.h"
#include "pomdog/basic/types.h"
#include "pomdog/network/trust_store.h"
#include "pomdog/utility/path_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <array>
#include <fstream>
#include <iterator>
#include <span>
#include <string>
#include <string_view>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

// NOTE: Suppress C4866 warning for doctest expressions in this file
POMDOG_MSVC_SUPPRESS_WARNING(4866)

using pomdog::u8;
using pomdog::network::createTrustStore;
using pomdog::network::TrustStoreDesc;
using pomdog::testing::KeyHash;
using pomdog::testing::makeDistrustedKeysFile;

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

/// Creates a trust store from roots and the key list keys.
[[nodiscard]] auto
create(std::string_view roots, std::span<const u8> keys = {})
{
    TrustStoreDesc desc;
    desc.rootCertificatesPEM = asBytes(roots);
    desc.distrustedKeys = keys;
    return createTrustStore(desc);
}

} // namespace

TEST_CASE("createTrustStore reads the engine root certificates" * doctest::test_suite(pomdog::testing::pomdogDirTestSuite))
{
    auto store = create(readRootCertificates());
    REQUIRE(store.has_value());
    REQUIRE(*store != nullptr);
}

#if defined(POMDOG_PLATFORM_EMSCRIPTEN)
TEST_CASE("createTrustStore reads neither the roots nor the keys on the web")
{
    const auto keys = std::vector<u8>{1, 2, 3};
    auto store = create("no certificate", keys);
    REQUIRE(store.has_value());
    REQUIRE(*store != nullptr);
}
#else
TEST_CASE("createTrustStore fails without a certificate")
{
    REQUIRE_FALSE(create("").has_value());
    REQUIRE_FALSE(create("no certificate").has_value());
}

TEST_CASE("createTrustStore fails when a certificate cannot be parsed" * doctest::test_suite(pomdog::testing::pomdogDirTestSuite))
{
    // NOTE: The last block decodes to bytes that are not a certificate.
    const auto pem = readRootCertificates() + "-----BEGIN CERTIFICATE-----\nAAAA\n-----END CERTIFICATE-----\n";
    auto store = create(pem);
    REQUIRE_FALSE(store.has_value());
    REQUIRE(store.error() != nullptr);
}

TEST_CASE("createTrustStore reads distrusted keys" * doctest::test_suite(pomdog::testing::pomdogDirTestSuite))
{
    const auto roots = readRootCertificates();

    SUBCASE("sorted keys")
    {
        KeyHash first = {};
        KeyHash second = {};
        first.fill(0x11);
        second.fill(0x22);
        const auto hashes = std::to_array({first, second});
        const auto keys = makeDistrustedKeysFile(hashes);
        REQUIRE(create(roots, keys).has_value());
    }
    SUBCASE("an empty list")
    {
        const auto keys = makeDistrustedKeysFile(std::vector<u8>{});
        REQUIRE(create(roots, keys).has_value());
    }
}

TEST_CASE("createTrustStore fails on invalid distrusted keys" * doctest::test_suite(pomdog::testing::pomdogDirTestSuite))
{
    const auto roots = readRootCertificates();

    SUBCASE("not a .dkeys file")
    {
        const auto keys = std::vector<u8>{1, 2, 3, 4, 5, 6, 7, 8};
        REQUIRE_FALSE(create(roots, keys).has_value());
    }
    SUBCASE("a size that is not a multiple of 32")
    {
        const auto keys = makeDistrustedKeysFile(std::vector<u8>(31, 0x11));
        REQUIRE_FALSE(create(roots, keys).has_value());
    }
    SUBCASE("unsorted keys")
    {
        KeyHash first = {};
        KeyHash second = {};
        first.fill(0x22);
        second.fill(0x11);
        const auto hashes = std::to_array({first, second});
        const auto keys = makeDistrustedKeysFile(hashes);
        REQUIRE_FALSE(create(roots, keys).has_value());
    }
    SUBCASE("duplicate keys")
    {
        KeyHash key = {};
        key.fill(0x11);
        const auto hashes = std::to_array({key, key});
        const auto keys = makeDistrustedKeysFile(hashes);
        REQUIRE_FALSE(create(roots, keys).has_value());
    }
}
#endif
