// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include <string>

namespace Pomdog {

struct POMDOG_EXPORT FileSystem {
    static bool CreateDirectory(std::string const& path);

    static bool CreateDirectories(std::string const& path);

    static bool Exists(std::string const& path);

    static bool IsDirectory(std::string const& path);

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
