// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Utility/FileSystem.hpp"
#include "../Utility/ErrorHelper.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include "Pomdog/Utility/PathHelper.hpp"
#include <filesystem>

namespace Pomdog {

bool FileSystem::CreateDirectory(const std::string& path)
{
    // 'CreateDirectory' means 'CreateDirectoryA' in Win32.
    POMDOG_ASSERT(!path.empty());
    return std::filesystem::create_directory(path);
}

} // namespace Pomdog

#include "Pomdog/Platform/Win32/PrerequisitesWin32.hpp"
#include <Shlobj.h>

namespace Pomdog {

bool FileSystem::CreateDirectories(const std::string& path)
{
    POMDOG_ASSERT(!path.empty());
    return std::filesystem::create_directories(path);
}

bool FileSystem::Exists(const std::string& path)
{
    POMDOG_ASSERT(!path.empty());
    return std::filesystem::exists(path);
}

bool FileSystem::IsDirectory(const std::string& path)
{
    POMDOG_ASSERT(!path.empty());
    return std::filesystem::is_directory(path);
}

std::tuple<std::size_t, std::unique_ptr<Error>>
FileSystem::GetFileSize(const std::string& path)
{
    struct ::stat st;
    int rc = ::stat(path.data(), &st);

    if (rc != 0) {
        auto errorCode = Detail::ToErrc(errno);
        return std::make_tuple(0, Errors::New(errorCode, "::stat() failed"));
    }
    return std::make_tuple(st.st_size, nullptr);
}

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

std::string FileSystem::GetResourceDirectoryPath()
{
    auto currentDirectory = std::filesystem::current_path();
    return currentDirectory.string();
}

std::string FileSystem::GetTempDirectoryPath()
{
    auto path = std::filesystem::temp_directory_path();
    return path.string();
}

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

} // namespace Pomdog
