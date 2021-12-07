// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/utility/errors.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail::Win32 {

/// Create a new directory with the specified path.
[[nodiscard]] std::unique_ptr<Error>
CreateNewDirectory(const std::string& path) noexcept;

/// Recursively create a new directory with the specified path.
[[nodiscard]] std::unique_ptr<Error>
CreateDirectories(const std::string& path) noexcept;

/// Returns true if the file exists, false otherwise.
[[nodiscard]] bool
Exists(const std::string& path) noexcept;

/// Returns true if the file is directory, false otherwise.
[[nodiscard]] bool
IsDirectory(const std::string& path) noexcept;

/// Gets the size of the specified file.
[[nodiscard]] std::tuple<std::size_t, std::unique_ptr<Error>>
GetFileSize(const std::string& path) noexcept;

/// Gets the location of the "LocalAppData" directory.
[[nodiscard]] std::tuple<std::string, std::unique_ptr<Error>>
GetLocalAppDataDirectoryPath() noexcept;

/// Gets the location of the "AppData" directory.
[[nodiscard]] std::tuple<std::string, std::unique_ptr<Error>>
GetAppDataDirectoryPath() noexcept;

/// Gets the resource directory.
[[nodiscard]] std::tuple<std::string, std::unique_ptr<Error>>
GetResourceDirectoryPath() noexcept;

/// Gets the temporary directory.
[[nodiscard]] std::tuple<std::string, std::unique_ptr<Error>>
GetTempDirectoryPath() noexcept;

/// Gets the current working directory.
[[nodiscard]] std::tuple<std::string, std::unique_ptr<Error>>
GetCurrentWorkingDirectory() noexcept;

} // namespace Pomdog::Detail::Win32
