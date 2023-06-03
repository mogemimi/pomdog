// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::win32 {

/// Create a new directory with the specified path.
[[nodiscard]] std::unique_ptr<Error>
createNewDirectory(const std::string& path) noexcept;

/// Recursively create a new directory with the specified path.
[[nodiscard]] std::unique_ptr<Error>
createDirectories(const std::string& path) noexcept;

/// Returns true if the file exists, false otherwise.
[[nodiscard]] bool
exists(const std::string& path) noexcept;

/// Returns true if the file is directory, false otherwise.
[[nodiscard]] bool
isDirectory(const std::string& path) noexcept;

/// Gets the size of the specified file.
[[nodiscard]] std::tuple<std::size_t, std::unique_ptr<Error>>
getFileSize(const std::string& path) noexcept;

/// Gets the location of the "LocalAppData" directory.
[[nodiscard]] std::tuple<std::string, std::unique_ptr<Error>>
getLocalAppDataDirectoryPath() noexcept;

/// Gets the location of the "AppData" directory.
[[nodiscard]] std::tuple<std::string, std::unique_ptr<Error>>
getAppDataDirectoryPath() noexcept;

/// Gets the resource directory.
[[nodiscard]] std::tuple<std::string, std::unique_ptr<Error>>
getResourceDirectoryPath() noexcept;

/// Gets the temporary directory.
[[nodiscard]] std::tuple<std::string, std::unique_ptr<Error>>
getTempDirectoryPath() noexcept;

/// Gets the current working directory.
[[nodiscard]] std::tuple<std::string, std::unique_ptr<Error>>
getCurrentWorkingDirectory() noexcept;

} // namespace pomdog::detail::win32
