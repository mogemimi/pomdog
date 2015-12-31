// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#include "Pomdog/Utility/FileSystem.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include "Pomdog/Utility/PathHelper.hpp"
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

namespace Pomdog {
namespace Detail {
//-----------------------------------------------------------------------
bool FileSystem::CreateDirectory(std::string const& path)
{
    // 'CreateDirectory' means 'CreateDirectoryA' in Win32.
    POMDOG_ASSERT(!path.empty());
    return fs::create_directory(path);
}
//-----------------------------------------------------------------------
} // namespace Detail
} // namespace Pomdog

#include "Pomdog/Platform/Win32/PrerequisitesWin32.hpp"
#include <Shlobj.h>

namespace Pomdog {
namespace Detail {
//-----------------------------------------------------------------------
bool FileSystem::CreateDirectories(std::string const& path)
{
    POMDOG_ASSERT(!path.empty());
    return fs::create_directories(path);
}
//-----------------------------------------------------------------------
bool FileSystem::Exists(std::string const& path)
{
    POMDOG_ASSERT(!path.empty());
    return fs::exists(path);
}
//-----------------------------------------------------------------------
bool FileSystem::IsDirectory(std::string const& path)
{
    POMDOG_ASSERT(!path.empty());
    return fs::is_directory(path);
}
//-----------------------------------------------------------------------
std::string FileSystem::GetLocalAppDataDirectoryPath()
{
    TCHAR directoryName[MAX_PATH];

    auto hr = SHGetFolderPath(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, directoryName);
    if (FAILED(hr)) {
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to get AppData path.");
    }

    ///@todo FIXME
    constexpr auto productName = "Pomdog";

    std::string localAppData(directoryName);
    return PathHelper::Join(localAppData, productName);
}
//-----------------------------------------------------------------------
std::string FileSystem::GetAppDataDirectoryPath()
{
    TCHAR directoryName[MAX_PATH];

    auto hr = SHGetFolderPath(nullptr, CSIDL_APPDATA, nullptr, 0, directoryName);
    if (FAILED(hr)) {
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to get AppData path.");
    }

    ///@todo FIXME
    constexpr auto productName = "Pomdog";

    std::string appData(directoryName);
    return PathHelper::Join(appData, productName);
}
//-----------------------------------------------------------------------
std::string FileSystem::GetResourceDirectoryPath()
{
    auto currentDirectory = fs::current_path();
    return currentDirectory.string();
}
//-----------------------------------------------------------------------
std::string FileSystem::GetTempDirectoryPath()
{
    auto path = fs::temp_directory_path();
    return path.string();
}
//-----------------------------------------------------------------------
std::string FileSystem::GetCurrentWorkingDirectory()
{
    TCHAR directory[MAX_PATH + 1];
    const auto length = ::GetCurrentDirectory(MAX_PATH, directory);
    if (length > 0) {
        std::string result(directory, length);
        return std::move(result);
    }
    return {};
}
//-----------------------------------------------------------------------
} // namespace Detail
} // namespace Pomdog
