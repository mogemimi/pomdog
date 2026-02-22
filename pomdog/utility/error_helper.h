// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <system_error>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::errors {

/// Converts the given error code such as `errno` to `std::errc`.
[[nodiscard]] std::errc
toErrc(int err) noexcept;

/// Returns an error with the given error code.
[[nodiscard]] std::unique_ptr<Error>
fromErrc(const std::errc& errorCode) noexcept;

/// Returns an error with the given error code and additional message.
[[nodiscard]] std::unique_ptr<Error>
fromErrc(const std::errc& errorCode, std::string&& message) noexcept;

} // namespace pomdog::errors
