// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "FileSystemWin32.hpp"
#include "../Utility/ErrorHelper.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Platform/Win32/PrerequisitesWin32.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/PathHelper.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <Shlobj.h>
#include <filesystem>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail::Win32 {

std::unique_ptr<Error>
CreateNewDirectory(const std::string& path) noexcept
{
    POMDOG_ASSERT(!path.empty());
    std::error_code err;
    if (!std::filesystem::create_directory(path, err)) {
        return Errors::New("create_directory() failed: " + err.message());
    }
    return nullptr;
}

std::unique_ptr<Error>
CreateDirectories(const std::string& path) noexcept
{
    POMDOG_ASSERT(!path.empty());
    std::error_code err;
    if (!std::filesystem::create_directories(path, err)) {
        return Errors::New("create_directories() failed: " + err.message());
    }
    return nullptr;
}

bool Exists(const std::string& path) noexcept
{
    POMDOG_ASSERT(!path.empty());
    return std::filesystem::exists(path);
}

bool IsDirectory(const std::string& path) noexcept
{
    POMDOG_ASSERT(!path.empty());
    return std::filesystem::is_directory(path);
}

std::tuple<std::size_t, std::unique_ptr<Error>>
GetFileSize(const std::string& path) noexcept
{
    struct ::stat st;
    if (int result = ::stat(path.data(), &st); result != 0) {
        auto err = Detail::ToErrc(errno);
        return std::make_tuple(0, Errors::New(err, "::stat() failed"));
    }
    return std::make_tuple(st.st_size, nullptr);
}

std::tuple<std::string, std::unique_ptr<Error>>
GetLocalAppDataDirectoryPath() noexcept
{
    char directory[MAX_PATH];
    directory[MAX_PATH - 1] = '\0';

    auto hr = SHGetFolderPathA(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, directory);
    if (FAILED(hr)) {
        return std::make_tuple("", Errors::New("SHGetFolderPath(CSIDL_LOCAL_APPDATA) failed: " + std::to_string(hr)));
    }

    // FIXME: Change so that the user can specify any application name.
    constexpr auto productName = "Pomdog";

    auto result = PathHelper::Join(directory, productName);
    return std::make_tuple(std::move(result), nullptr);
}

std::tuple<std::string, std::unique_ptr<Error>>
GetAppDataDirectoryPath() noexcept
{
    char directory[MAX_PATH];
    directory[MAX_PATH - 1] = '\0';

    auto hr = SHGetFolderPathA(nullptr, CSIDL_APPDATA, nullptr, 0, directory);
    if (FAILED(hr)) {
        return std::make_tuple("", Errors::New("SHGetFolderPath(CSIDL_APPDATA) failed: " + std::to_string(hr)));
    }

    // FIXME: Change so that the user can specify any application name.
    constexpr auto productName = "Pomdog";

    auto result = PathHelper::Join(directory, productName);
    return std::make_tuple(std::move(result), nullptr);
}

std::tuple<std::string, std::unique_ptr<Error>>
GetResourceDirectoryPath() noexcept
{
    auto currentDirectory = std::filesystem::current_path();
    return std::make_tuple(currentDirectory.string(), nullptr);
}

std::tuple<std::string, std::unique_ptr<Error>>
GetTempDirectoryPath() noexcept
{
    auto path = std::filesystem::temp_directory_path();
    return std::make_tuple(path.string(), nullptr);
}

std::tuple<std::string, std::unique_ptr<Error>>
GetCurrentWorkingDirectory() noexcept
{
    char directory[MAX_PATH];
    directory[MAX_PATH - 1] = '\0';

    const auto length = ::GetCurrentDirectoryA(MAX_PATH - 1, directory);
    if (length <= 0) {
        return std::make_tuple("", Errors::New("GetCurrentDirectoryA() failed"));
    }
    std::string result(directory, length);
    return std::make_tuple(std::move(result), nullptr);
}

} // namespace Pomdog::Detail::Win32
