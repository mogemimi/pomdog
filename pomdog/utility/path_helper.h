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

/// Joins two paths into a single path, adding a separator if necessary,
/// and normalizes the result by removing "." elements and resolving ".." elements.
/// This is similar to Go's filepath.Join.
/// Empty strings are ignored except when both are empty (returns "").
/// Examples: join("a/b", "c") -> "a/b/c" (Unix) or "a\\b\\c" (Windows)
///           join("a/b", "../c") -> "a/c"
[[nodiscard]] POMDOG_EXPORT std::string
join(std::string_view path1, std::string_view path2) noexcept;

/// Joins two paths into a single path using Windows-style separators (\\) and normalizes the result.
/// This function is provided for cross-platform testing and accepts both / and \\ as separators.
/// Examples: joinWindows("a/b", "c") -> "a\\b\\c"
///           joinWindows("a/b", "../c") -> "a\\c"
[[nodiscard]] POMDOG_EXPORT std::string
joinWindows(std::string_view path1, std::string_view path2) noexcept;

/// Joins two paths into a single path using Unix-style separators (/) and normalizes the result.
/// This function is provided for cross-platform testing.
/// Examples: joinUnix("a/b", "c") -> "a/b/c"
///           joinUnix("a/b", "../c") -> "a/c"
[[nodiscard]] POMDOG_EXPORT std::string
joinUnix(std::string_view path1, std::string_view path2) noexcept;

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

/// Returns the shortest path name lexically equivalent to the given path.
/// This is similar to Go's filepath.Clean.
/// It removes "." elements, resolves ".." elements, and converts multiple
/// consecutive separators to single ones.
/// Returns "." if the result is empty.
/// Examples: normalize("a/./b") -> "a/b" (Unix) or "a\\b" (Windows)
///           normalize("a/../b") -> "b"
///           normalize("./a") -> "a"
[[nodiscard]] POMDOG_EXPORT std::string
normalize(std::string_view path) noexcept;

/// Returns the shortest path using Windows-style separators (\\).
/// This function is provided for cross-platform testing and accepts both / and \\ as separators.
/// Leading / is converted to \\.
/// Examples: normalizeWindows("a/./b") -> "a\\b"
///           normalizeWindows("/usr/local") -> "\\usr\\local"
[[nodiscard]] POMDOG_EXPORT std::string
normalizeWindows(std::string_view path) noexcept;

/// Returns the shortest path using Unix-style separators (/).
/// This function is provided for cross-platform testing.
/// Examples: normalizeUnix("a/./b") -> "a/b"
///           normalizeUnix("/usr/local") -> "/usr/local"
[[nodiscard]] POMDOG_EXPORT std::string
normalizeUnix(std::string_view path) noexcept;

/// Returns the path of replacing each separator character with a slash ('/').
[[nodiscard]] POMDOG_EXPORT std::string
toSlash(std::string_view path) noexcept;

/// Returns a relative path from the start directory to the target path.
/// This is similar to Go's filepath.Rel.
/// Both paths are normalized before calculating the relative path.
/// Returns "." if both normalized paths are equal.
/// Examples: relative("/a/b/c", "/a/b") -> "c"
///           relative("/a/b", "/a/b/c") -> ".."
///           relative("/a/x", "/a/b") -> "../x"
[[nodiscard]] POMDOG_EXPORT std::string
relative(std::string_view path, std::string_view start) noexcept;

/// Returns a relative path using Windows-style separators (\\).
/// This function is provided for cross-platform testing.
/// Examples: relativeWindows("/a/b/c", "/a/b") -> "c"
///           relativeWindows("/a/b", "/a/b/c") -> ".."
[[nodiscard]] POMDOG_EXPORT std::string
relativeWindows(std::string_view path, std::string_view start) noexcept;

/// Returns a relative path using Unix-style separators (/).
/// This function is provided for cross-platform testing.
/// Examples: relativeUnix("/a/b/c", "/a/b") -> "c"
///           relativeUnix("/a/b", "/a/b/c") -> ".."
[[nodiscard]] POMDOG_EXPORT std::string
relativeUnix(std::string_view path, std::string_view start) noexcept;

/// Returns true if the path is absolute, false otherwise.
/// This is similar to Go's filepath.IsAbs.
/// On Windows: requires drive letter (C:, D:) or UNC path (\\\\ or //)
///             Single / or \\ is considered relative.
/// On Unix: requires leading slash (/)
/// Examples: isAbsolute("/usr/bin") -> true (Unix)
///           isAbsolute("C:/Windows") -> true (Windows)
///           isAbsolute("relative/path") -> false
[[nodiscard]] POMDOG_EXPORT bool
isAbsolute(std::string_view path) noexcept;

/// Returns true if the path is absolute according to Windows rules.
/// This function is provided for cross-platform testing and accepts both / and \\ as separators.
/// Requires drive letter (C:, D:) or UNC path (\\\\ or //).
/// Single / or \\ is considered relative on Windows.
/// Examples: isAbsoluteWindows("C:/path") -> true
///           isAbsoluteWindows("//server/share") -> true
///           isAbsoluteWindows("/path") -> false
[[nodiscard]] POMDOG_EXPORT bool
isAbsoluteWindows(std::string_view path) noexcept;

/// Returns true if the path is absolute according to Unix rules.
/// This function is provided for cross-platform testing.
/// Requires leading slash (/).
/// Examples: isAbsoluteUnix("/usr/bin") -> true
///           isAbsoluteUnix("relative/path") -> false
[[nodiscard]] POMDOG_EXPORT bool
isAbsoluteUnix(std::string_view path) noexcept;

} // namespace pomdog::filepaths
