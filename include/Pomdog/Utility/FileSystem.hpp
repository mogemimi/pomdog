// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include <string>

namespace Pomdog {

struct POMDOG_EXPORT FileSystem final {
    static bool CreateDirectory(const std::string& path);

    static bool CreateDirectories(const std::string& path);

    static bool Exists(const std::string& path);

    static bool IsDirectory(const std::string& path);

    // Windows:
    // - "<user name>\Local Settings\Applicaiton Data\Pomdog"
    // - "C:\Users\<user name>\AppData\Local\Pomdog"
    static std::string GetLocalAppDataDirectoryPath();

    // Windows:
    // - "<user name>\Application Data\Pomdog"
    // - "C:\Users\<user name>\AppData\Roaming\Pomdog"
    static std::string GetAppDataDirectoryPath();

    // Windows:
    // - "<current directory>"
    static std::string GetResourceDirectoryPath();

    // Windows:
    // - "C:\Users\<user name>\AppData\Local\Temp"
    static std::string GetTempDirectoryPath();

    static std::string GetCurrentWorkingDirectory();
};

} // namespace Pomdog
