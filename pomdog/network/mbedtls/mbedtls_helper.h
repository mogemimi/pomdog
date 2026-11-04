// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <expected>
#include <memory>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {

/// Returns the description of the Mbed TLS error code err.
[[nodiscard]] std::string
mbedTLSErrorToString(int err);

/// Initializes PSA crypto, which certificate parsing and TLS use.
///
/// It succeeds again when PSA is already initialized, and PSA stays
/// initialized for the process lifetime.
[[nodiscard]] std::expected<void, std::unique_ptr<Error>>
initializePSACrypto();

} // namespace pomdog::network::detail
