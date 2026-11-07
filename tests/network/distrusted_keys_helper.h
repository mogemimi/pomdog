// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "tests/testing/testing.h"
#include "pomdog/basic/flatbuffers_macros.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_BEGIN
#include "pomdogschemas/distrusted_keys_generated.h"
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_END

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <array>
#include <span>
#include <string_view>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

namespace pomdog::testing {

/// KeyHash is the SHA-256 hash of a public key, as a `.dkeys` file lists it.
using KeyHash = std::array<u8, 32>;

/// Returns a `.dkeys` file that lists bytes as its key hashes, as they are,
/// without sorting them.
[[nodiscard]] inline std::vector<u8>
makeDistrustedKeysFile(const std::vector<u8>& bytes)
{
    flatbuffers::FlatBufferBuilder builder;
    builder.Finish(pomdogschemas::CreateDistrustedKeysDirect(builder, &bytes));
    const auto buffer = builder.GetBufferSpan();
    return std::vector<u8>(buffer.begin(), buffer.end());
}

/// Returns a `.dkeys` file that lists hashes, which must be sorted.
[[nodiscard]] inline std::vector<u8>
makeDistrustedKeysFile(std::span<const KeyHash> hashes)
{
    std::vector<u8> bytes;
    for (const auto& hash : hashes) {
        bytes.insert(std::end(bytes), std::begin(hash), std::end(hash));
    }
    return makeDistrustedKeysFile(bytes);
}

/// Returns the key hash that hex, 64 hexadecimal digits, describes.
[[nodiscard]] inline KeyHash
parseKeyHash(std::string_view hex)
{
    REQUIRE(hex.size() == 64);
    const auto digit = [](char c) -> u8 {
        if ((c >= '0') && (c <= '9')) {
            return static_cast<u8>(c - '0');
        }
        if ((c >= 'a') && (c <= 'f')) {
            return static_cast<u8>((c - 'a') + 10);
        }
        REQUIRE(((c >= 'A') && (c <= 'F')));
        return static_cast<u8>((c - 'A') + 10);
    };
    KeyHash hash = {};
    for (std::size_t i = 0; i < hash.size(); ++i) {
        hash[i] = static_cast<u8>((digit(hex[i * 2]) << 4) | digit(hex[(i * 2) + 1]));
    }
    return hash;
}

} // namespace pomdog::testing
