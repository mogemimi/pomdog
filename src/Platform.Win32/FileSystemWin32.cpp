// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Utility/detail/FileSystem.hpp"
#include "Pomdog/Content/Utility/PathHelper.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
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

// Bug on Visual Studio 2015 RC.
// Please see https://connect.microsoft.com/VisualStudio/feedback/details/976983
#pragma warning(push)
#pragma warning(disable : 4091)
#include <Shlobj.h>
#pragma warning(pop)

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
} // namespace Detail
} // namespace Pomdog
