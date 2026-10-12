// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <memory>
#include <string>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::platformfs {

/// DirectoryCreation tells whether createDirectory() or createDirectories()
/// made the directory or found it already there.
enum class DirectoryCreation : u8 {
    /// The call created the directory.
    Created,

    /// A directory already existed at the path, and the call left it as it was.
    AlreadyExists,
};

/// Creates a directory at `path`, whose parent directory must exist.
///
/// Returns DirectoryCreation::AlreadyExists without an error when a directory
/// already exists at `path`, and an error when a file that is not a directory
/// does. The DirectoryCreation means nothing when the error is not null.
///
/// Call this function without calling exists() first. Another process can
/// create or remove the path between the check and the creation, which is a
/// time-of-check to time-of-use race; this function instead decides from the
/// result of the creation itself.
[[nodiscard]] POMDOG_EXPORT std::tuple<DirectoryCreation, std::unique_ptr<Error>>
createDirectory(const std::string& path) noexcept;

/// Creates a directory at `path`, along with its missing parent directories.
///
/// Returns DirectoryCreation::AlreadyExists without an error when a directory
/// already exists at `path`, and an error when a file that is not a directory
/// exists at `path` or in place of one of its parents. The DirectoryCreation
/// means nothing when the error is not null.
///
/// Call this function without calling exists() first, for the same reason as
/// createDirectory().
[[nodiscard]] POMDOG_EXPORT std::tuple<DirectoryCreation, std::unique_ptr<Error>>
createDirectories(const std::string& path) noexcept;

/// Returns true if the file exists, false otherwise.
[[nodiscard]] POMDOG_EXPORT bool
exists(const std::string& path) noexcept;

/// Returns true if the file is directory, false otherwise.
[[nodiscard]] POMDOG_EXPORT bool
isDirectory(const std::string& path) noexcept;

/// Gets the size of the specified file.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::size_t, std::unique_ptr<Error>>
getFileSize(const std::string& path) noexcept;

/// Returns the canonical path that is both absolute and unique.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::string, std::unique_ptr<Error>>
getCanonicalPath(const std::string& path) noexcept;

/// Gets the location of the "LocalAppData" directory.
///
/// Windows:
/// - e.g. "C:/Users/<user name>/AppData/Local/Pomdog"
[[nodiscard]] POMDOG_EXPORT std::tuple<std::string, std::unique_ptr<Error>>
getLocalAppDataDirectoryPath() noexcept;

/// Gets the location of the "AppData" directory.
///
/// Windows:
/// - e.g. "<user name>/Application Data/Pomdog"
/// - e.g. "C:/Users/<user name>/AppData/Roaming/Pomdog"
[[nodiscard]] POMDOG_EXPORT std::tuple<std::string, std::unique_ptr<Error>>
getAppDataDirectoryPath() noexcept;

/// Gets the resource directory.
///
/// Windows:
/// - Returns the directory containing the executable file.
/// - e.g. "C:/path/to/executable/directory"
[[nodiscard]] POMDOG_EXPORT std::tuple<std::string, std::unique_ptr<Error>>
getResourceDirectoryPath() noexcept;

/// Gets the temporary directory.
///
/// Windows:
/// - e.g. "C:/Users/<user name>/AppData/Local/Temp"
[[nodiscard]] POMDOG_EXPORT std::tuple<std::string, std::unique_ptr<Error>>
getTempDirectoryPath() noexcept;

/// Gets the current working directory.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::string, std::unique_ptr<Error>>
getCurrentWorkingDirectory() noexcept;

} // namespace pomdog::platformfs
