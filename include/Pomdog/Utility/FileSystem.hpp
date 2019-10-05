// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Utility/Errors.hpp"
#include <string>

namespace Pomdog {

struct POMDOG_EXPORT FileSystem final {
    /// Create a new directory with the specified path.
    [[nodiscard]] static bool
    CreateDirectory(const std::string& path);

    /// Recursively create a new directory with the specified path.
    [[nodiscard]] static bool
    CreateDirectories(const std::string& path);

    /// Returns true if the file exists, false otherwise.
    [[nodiscard]] static bool
    Exists(const std::string& path);

    /// Returns true if the file is directory, false otherwise.
    [[nodiscard]] static bool
    IsDirectory(const std::string& path);

    /// Gets the size of the specified file.
    [[nodiscard]] static std::tuple<std::size_t, std::shared_ptr<Error>>
    GetFileSize(const std::string& path);

    // Windows:
    // - "<user name>\Local Settings\Applicaiton Data\Pomdog"
    // - "C:\Users\<user name>\AppData\Local\Pomdog"
    [[nodiscard]] static std::string
    GetLocalAppDataDirectoryPath();

    // Windows:
    // - "<user name>\Application Data\Pomdog"
    // - "C:\Users\<user name>\AppData\Roaming\Pomdog"
    [[nodiscard]] static std::string
    GetAppDataDirectoryPath();

    // Windows:
    // - "<current directory>"
    [[nodiscard]] static std::string
    GetResourceDirectoryPath();

    // Windows:
    // - "C:\Users\<user name>\AppData\Local\Temp"
    [[nodiscard]] static std::string
    GetTempDirectoryPath();

    /// Gets the current working directory.
    [[nodiscard]] static std::string
    GetCurrentWorkingDirectory();
};

} // namespace Pomdog
