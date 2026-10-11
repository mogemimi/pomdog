// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::vfs {

/// Returns an error that reports that no file exists, with `message` naming the file.
///
/// A mount volume returns this error when it has no file at the requested path or
/// hash key, so that the VFS tries the next volume instead of failing.
[[nodiscard]] POMDOG_EXPORT std::unique_ptr<Error>
makeNotFoundError(std::string&& message) noexcept;

/// Returns true when `err`, or an error that it wraps, comes from makeNotFoundError().
[[nodiscard]] POMDOG_EXPORT bool
isNotFound(const std::unique_ptr<Error>& err) noexcept;

} // namespace pomdog::vfs
