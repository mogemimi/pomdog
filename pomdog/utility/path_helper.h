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
/// This is similar to Go's filepath.Base, but returns "" for trailing separators.
/// Handles both Windows-style (\\) and Unix-style (/) path separators.
/// Examples: getBaseName("a/b/c") -> "c"
///           getBaseName("a\\b\\c") -> "c"
///           getBaseName("a/b/") -> ""
///           getBaseName("a") -> "a"
///           getBaseName("") -> ""
/// If you want to avoid allocations, use getBaseNameAsView instead.
/// When using getBaseNameAsView, ensure that the returned string view has the same lifetime
/// as the original path string. A common mistake is to use `getBaseNameAsView(normalize(path))`.
/// Since `normalize(path)` returns a temporary string, the return value of getBaseNameAsView
/// will be invalid. In that case, do something like:
///     ```cpp
///     auto normalizedPath = normalize(path);
///     auto baseName = getBaseNameAsView(normalizedPath);
///     ```
/// or use:
///     ```
///     getBaseName(normalize(path))
///     ```
[[nodiscard]] POMDOG_EXPORT std::string
getBaseName(std::string_view path) noexcept;

/// Returns the last element of path as a string_view without allocating a new string.
/// Handles both Windows-style (\\) and Unix-style (/) path separators.
/// @warning The returned string_view references the original path string.
/// Ensure the original string outlives the returned view.
/// Do not use with temporary strings, e.g., `getBaseNameAsView(normalize(path))` is undefined behavior.
/// Examples: getBaseNameAsView("a/b/c") -> "c"
///           getBaseNameAsView("a\\b\\c") -> "c"
[[nodiscard]] POMDOG_EXPORT std::string_view
getBaseNameAsView(std::string_view path) noexcept;

/// Returns the directory name of path.
/// This is similar to Go's filepath.Dir.
/// Handles both Windows-style (\\) and Unix-style (/) path separators.
/// Examples: getDirectoryName("a/b/c") -> "a/b"
///           getDirectoryName("a\\b\\c") -> "a\\b"
///           getDirectoryName("a/b/") -> "a/b"
///           getDirectoryName("/foo.txt") -> "/"
///           getDirectoryName("foo") -> ""
/// If you want to avoid allocations, use getDirectoryNameAsView instead.
/// When using getDirectoryNameAsView, ensure that the returned string view has the same lifetime
/// as the original path string. A common mistake is to use `getDirectoryNameAsView(normalize(path))`.
/// Since `normalize(path)` returns a temporary string, the return value of getDirectoryNameAsView
/// will be invalid. In that case, do something like:
///     ```cpp
///     auto normalizedPath = normalize(path);
///     auto directoryName = getDirectoryNameAsView(normalizedPath);
///     ```
/// or use:
///     ```
///     getDirectoryName(normalize(path))
///     ```
[[nodiscard]] POMDOG_EXPORT std::string
getDirectoryName(std::string_view path) noexcept;

/// Returns the directory name of path as a string_view without allocating a new string.
/// Handles both Windows-style (\\) and Unix-style (/) path separators.
/// @warning The returned string_view references the original path string.
/// Ensure the original string outlives the returned view.
/// Do not use with temporary strings, e.g., `getDirectoryNameAsView(normalize(path))` is undefined behavior.
/// Examples: getDirectoryNameAsView("a/b/c") -> "a/b"
///           getDirectoryNameAsView("a\\b\\c") -> "a\\b"
[[nodiscard]] POMDOG_EXPORT std::string_view
getDirectoryNameAsView(std::string_view path) noexcept;

/// Returns the directory name of path and the last element of path.
/// This is similar to Go's filepath.Split, but the directory does not include a trailing separator.
/// Handles both Windows-style (\\) and Unix-style (/) path separators.
/// Examples: split("a/b/c") -> ("a/b", "c")
///           split("a\\b\\c") -> ("a\\b", "c")
///           split("a/b/") -> ("a/b", "")
///           split("a") -> ("", "a")
///           split("") -> ("", "")
/// If you want to avoid allocations, use splitAsView instead.
/// When using splitAsView, ensure that the returned string views have the same lifetime
/// as the original path string. A common mistake is to use `splitAsView(normalize(path))`.
/// Since `normalize(path)` returns a temporary string, the return value of splitAsView
/// will be invalid. In that case, do something like:
///     ```cpp
///     auto normalizedPath = normalize(path);
///     auto [directoryName, baseName] = splitAsView(normalizedPath);
///     ```
/// or use:
///     ```
///     split(normalize(path))
///     ```
[[nodiscard]] POMDOG_EXPORT std::tuple<std::string, std::string>
split(std::string_view path) noexcept;

/// Returns the directory name of path and the last element of path as string_views without allocating new strings.
/// Handles both Windows-style (\\) and Unix-style (/) path separators.
/// @warning The returned string_views reference the original path string.
/// Ensure the original string outlives the returned views.
/// Do not use with temporary strings, e.g., `splitAsView(normalize(path))` is undefined behavior.
/// Examples: splitAsView("a/b/c") -> ("a/b", "c")
///           splitAsView("a\\b\\c") -> ("a\\b", "c")
[[nodiscard]] POMDOG_EXPORT std::tuple<std::string_view, std::string_view>
splitAsView(std::string_view path) noexcept;

/// Returns the base name of path and file name extension.
/// This is similar to Go's filepath.Ext.
/// The extension splitting uses '.' and is independent of path separators.
/// Examples: splitExtension("a/b/c.txt") -> ("a/b/c", "txt")
///           splitExtension("a\\b\\c.txt") -> ("a\\b\\c", "txt")
///           splitExtension("a/b/c") -> ("a/b/c", "")
///           splitExtension("a/b/c.") -> ("a/b/c", "")
/// If you want to avoid allocations, use splitExtensionAsView instead.
/// When using splitExtensionAsView, ensure that the returned string views have the same lifetime
/// as the original path string. A common mistake is to use `splitExtensionAsView(normalize(path))`.
/// Since `normalize(path)` returns a temporary string, the return value of splitExtensionAsView
/// will be invalid. In that case, do something like:
///     ```cpp
///     auto normalizedPath = normalize(path);
///     auto [baseName, extension] = splitExtensionAsView(normalizedPath);
///     ```
/// or use:
///     ```
///     splitExtension(normalize(path))
///     ```
[[nodiscard]] POMDOG_EXPORT std::tuple<std::string, std::string>
splitExtension(std::string_view path) noexcept;

/// Returns the base name of path and file name extension as string_views without allocating new strings.
/// @warning The returned string_views reference the original path string.
/// Ensure the original string outlives the returned views.
/// Do not use with temporary strings, e.g., `splitExtensionAsView(normalize(path))` is undefined behavior.
/// Examples: splitExtensionAsView("a/b/c.txt") -> ("a/b/c", "txt")
///           splitExtensionAsView("a/b/c") -> ("a/b/c", "")
[[nodiscard]] POMDOG_EXPORT std::tuple<std::string_view, std::string_view>
splitExtensionAsView(std::string_view path) noexcept;

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
