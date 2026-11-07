// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <expected>
#include <memory>
#include <span>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {

/// Returns the description of the Mbed TLS error code err.
[[nodiscard]] std::string
mbedTLSErrorToString(int err);

/// SHA256Hash is a SHA-256 hash.
using SHA256Hash = std::array<u8, 32>;

/// Initializes PSA crypto, which certificate parsing and TLS use.
///
/// It succeeds again when PSA is already initialized, and PSA stays
/// initialized for the process lifetime.
[[nodiscard]] std::expected<void, std::unique_ptr<Error>>
initializePSACrypto();

/// Returns the SHA-256 hash of data. PSA crypto must be initialized.
[[nodiscard]] std::expected<SHA256Hash, std::unique_ptr<Error>>
computeSHA256(std::span<const u8> data);

/// SHA1Hash is a SHA-1 hash.
using SHA1Hash = std::array<u8, 20>;

/// Returns the SHA-1 hash of data, which the WebSocket handshake uses. PSA
/// crypto must be initialized.
[[nodiscard]] std::expected<SHA1Hash, std::unique_ptr<Error>>
computeSHA1(std::span<const u8> data);

/// Fills output with random bytes from the generator of PSA crypto, which
/// must be initialized.
[[nodiscard]] std::expected<void, std::unique_ptr<Error>>
generateRandomBytes(std::span<u8> output);

} // namespace pomdog::network::detail
