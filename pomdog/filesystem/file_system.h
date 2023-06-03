// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <memory>
#include <string>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::FileSystem {

/// Create a new directory with the specified path.
[[nodiscard]] POMDOG_EXPORT std::unique_ptr<Error>
createNewDirectory(const std::string& path) noexcept;

/// Recursively create a new directory with the specified path.
[[nodiscard]] POMDOG_EXPORT std::unique_ptr<Error>
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
/// - e.g. "<user name>/Local Settings/Applicaiton Data/Pomdog"
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
/// - e.g. "<current directory>"
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

} // namespace pomdog::FileSystem
