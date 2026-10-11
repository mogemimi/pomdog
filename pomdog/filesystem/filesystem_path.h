// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <filesystem>
#include <string>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::platformfs {

/// Returns `path`, which is UTF-8, as a std::filesystem::path.
///
/// On Windows the result holds the path in UTF-16, so that the wide file APIs
/// open paths that the ANSI code page cannot represent. An invalid UTF-8
/// sequence becomes U+FFFD. Constructing std::filesystem::path from a
/// std::string directly would read the string in the ANSI code page instead.
[[nodiscard]] POMDOG_EXPORT std::filesystem::path
toFilesystemPath(std::string_view path) noexcept;

/// Returns `path` as a UTF-8 string.
[[nodiscard]] POMDOG_EXPORT std::string
toUTF8String(const std::filesystem::path& path) noexcept;

} // namespace pomdog::platformfs
