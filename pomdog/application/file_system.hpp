// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"
#include "pomdog/utility/errors.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <memory>
#include <string>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::FileSystem {

/// Create a new directory with the specified path.
[[nodiscard]] POMDOG_EXPORT std::unique_ptr<Error>
CreateNewDirectory(const std::string& path) noexcept;

/// Recursively create a new directory with the specified path.
[[nodiscard]] POMDOG_EXPORT std::unique_ptr<Error>
CreateDirectories(const std::string& path) noexcept;

/// Returns true if the file exists, false otherwise.
[[nodiscard]] POMDOG_EXPORT bool
Exists(const std::string& path) noexcept;

/// Returns true if the file is directory, false otherwise.
[[nodiscard]] POMDOG_EXPORT bool
IsDirectory(const std::string& path) noexcept;

/// Gets the size of the specified file.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::size_t, std::unique_ptr<Error>>
GetFileSize(const std::string& path) noexcept;

/// Returns the canonical path that is both absolute and unique.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::string, std::unique_ptr<Error>>
GetCanonicalPath(const std::string& path) noexcept;

/// Gets the location of the "LocalAppData" directory.
///
/// Windows:
/// - e.g. "<user name>/Local Settings/Applicaiton Data/Pomdog"
/// - e.g. "C:/Users/<user name>/AppData/Local/Pomdog"
[[nodiscard]] POMDOG_EXPORT std::tuple<std::string, std::unique_ptr<Error>>
GetLocalAppDataDirectoryPath() noexcept;

/// Gets the location of the "AppData" directory.
///
/// Windows:
/// - e.g. "<user name>/Application Data/Pomdog"
/// - e.g. "C:/Users/<user name>/AppData/Roaming/Pomdog"
[[nodiscard]] POMDOG_EXPORT std::tuple<std::string, std::unique_ptr<Error>>
GetAppDataDirectoryPath() noexcept;

/// Gets the resource directory.
///
/// Windows:
/// - e.g. "<current directory>"
[[nodiscard]] POMDOG_EXPORT std::tuple<std::string, std::unique_ptr<Error>>
GetResourceDirectoryPath() noexcept;

/// Gets the temporary directory.
///
/// Windows:
/// - e.g. "C:/Users/<user name>/AppData/Local/Temp"
[[nodiscard]] POMDOG_EXPORT std::tuple<std::string, std::unique_ptr<Error>>
GetTempDirectoryPath() noexcept;

/// Gets the current working directory.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::string, std::unique_ptr<Error>>
GetCurrentWorkingDirectory() noexcept;

} // namespace Pomdog::FileSystem
