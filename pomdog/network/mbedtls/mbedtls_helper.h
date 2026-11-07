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

} // namespace pomdog::network::detail
