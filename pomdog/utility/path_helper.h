// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string>
#include <string_view>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::filepaths {

/// Joins two paths into a single path, adding a separator if necessary.
[[nodiscard]] POMDOG_EXPORT std::string
join(std::string_view path1, std::string_view path2) noexcept;

/// Returns the last element of path.
[[nodiscard]] POMDOG_EXPORT std::string_view
getBaseName(std::string_view path) noexcept;

/// Returns the directory name of path.
[[nodiscard]] POMDOG_EXPORT std::string_view
getDirectoryName(std::string_view path) noexcept;

/// Returns the directory name of path and the last element of path.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::string_view, std::string_view>
split(std::string_view path) noexcept;

/// Returns the base name of path and file name extension.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::string_view, std::string_view>
splitExtension(std::string_view path) noexcept;

/// Returns the shortest path lexically.
[[nodiscard]] POMDOG_EXPORT std::string
normalize(std::string_view path) noexcept;

/// Returns the path of replacing each separator character with a slash ('/').
[[nodiscard]] POMDOG_EXPORT std::string
toSlash(std::string_view path) noexcept;

/// Returns a relative path from the start directory.
[[nodiscard]] POMDOG_EXPORT std::string
relative(std::string_view path, std::string_view start) noexcept;

/// Returns true if the path is absolute, false otherwise.
[[nodiscard]] POMDOG_EXPORT bool
isAbsolute(std::string_view path) noexcept;

} // namespace pomdog::filepaths
