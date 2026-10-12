// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/filesystem/win32/file_system_win32.h"
#include "pomdog/basic/win32/windows_headers.h"
#include "pomdog/filesystem/filesystem_path.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/error_helper.h"
#include "pomdog/utility/path_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <ShlObj.h>
#include <sys/stat.h>
#include <filesystem>
#include <system_error>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::win32 {
namespace {

/// Returns the UTF-8 form of the first `length` characters of `path`.
[[nodiscard]] std::string
toUTF8String(const wchar_t* path, std::size_t length) noexcept
{
    return platformfs::toUTF8String(std::filesystem::path{std::wstring{path, length}});
}

/// Returns the result of creating the directory at `path`, after
/// create_directory() or create_directories() returned `created`.
[[nodiscard]] std::tuple<platformfs::DirectoryCreation, std::unique_ptr<Error>>
toDirectoryCreation(bool created, const std::filesystem::path& fsPath, const std::string& path) noexcept
{
    if (created) {
        return {platformfs::DirectoryCreation::Created, nullptr};
    }

    // NOTE: The standard library returns false without an error when a file
    // already exists at the path, so tell a directory apart from the others.
    std::error_code err;
    if (!std::filesystem::is_directory(fsPath, err)) {
        return {platformfs::DirectoryCreation{}, errors::make("a file that is not a directory exists at the path: " + path)};
    }
    return {platformfs::DirectoryCreation::AlreadyExists, nullptr};
}

} // namespace

[[nodiscard]] std::tuple<platformfs::DirectoryCreation, std::unique_ptr<Error>>
createDirectory(const std::string& path) noexcept
{
    if (path.empty()) {
        return {platformfs::DirectoryCreation{}, errors::make("path is empty")};
    }

    const auto fsPath = platformfs::toFilesystemPath(path);
    std::error_code err;
    const bool created = std::filesystem::create_directory(fsPath, err);
    if (err) {
        return {platformfs::DirectoryCreation{}, errors::make("create_directory() failed: " + err.message() + ": " + path)};
    }
    return toDirectoryCreation(created, fsPath, path);
}

[[nodiscard]] std::tuple<platformfs::DirectoryCreation, std::unique_ptr<Error>>
createDirectories(const std::string& path) noexcept
{
    if (path.empty()) {
        return {platformfs::DirectoryCreation{}, errors::make("path is empty")};
    }

    const auto fsPath = platformfs::toFilesystemPath(path);
    std::error_code err;
    const bool created = std::filesystem::create_directories(fsPath, err);
    if (err) {
        return {platformfs::DirectoryCreation{}, errors::make("create_directories() failed: " + err.message() + ": " + path)};
    }
    return toDirectoryCreation(created, fsPath, path);
}

[[nodiscard]] bool
exists(const std::string& path) noexcept
{
    POMDOG_ASSERT(!path.empty());
    std::error_code err;
    return std::filesystem::exists(platformfs::toFilesystemPath(path), err);
}

[[nodiscard]] bool
isDirectory(const std::string& path) noexcept
{
    POMDOG_ASSERT(!path.empty());
    std::error_code err;
    return std::filesystem::is_directory(platformfs::toFilesystemPath(path), err);
}

[[nodiscard]] std::tuple<std::size_t, std::unique_ptr<Error>>
getFileSize(const std::string& path) noexcept
{
    struct ::_stat64 st = {};
    if (int result = ::_wstat64(platformfs::toFilesystemPath(path).c_str(), &st); result != 0) {
        const auto err = errors::toErrc(errno);
        return std::make_tuple(0, errors::fromErrc(err, "::_wstat64() failed"));
    }
    return std::make_tuple(static_cast<std::size_t>(st.st_size), nullptr);
}

[[nodiscard]] std::tuple<std::string, std::unique_ptr<Error>>
getLocalAppDataDirectoryPath() noexcept
{
    wchar_t directory[MAX_PATH] = {};

    auto hr = SHGetFolderPathW(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, directory);
    if (FAILED(hr)) {
        return std::make_tuple("", errors::make("SHGetFolderPath(CSIDL_LOCAL_APPDATA) failed: " + std::to_string(hr)));
    }

    // FIXME: Change so that the user can specify any application name.
    constexpr auto productName = "pomdog";

    auto result = filepaths::join(toUTF8String(directory, ::wcsnlen(directory, MAX_PATH)), productName);
    return std::make_tuple(std::move(result), nullptr);
}

[[nodiscard]] std::tuple<std::string, std::unique_ptr<Error>>
getAppDataDirectoryPath() noexcept
{
    wchar_t directory[MAX_PATH] = {};

    auto hr = SHGetFolderPathW(nullptr, CSIDL_APPDATA, nullptr, 0, directory);
    if (FAILED(hr)) {
        return std::make_tuple("", errors::make("SHGetFolderPath(CSIDL_APPDATA) failed: " + std::to_string(hr)));
    }

    // FIXME: Change so that the user can specify any application name.
    constexpr auto productName = "pomdog";

    auto result = filepaths::join(toUTF8String(directory, ::wcsnlen(directory, MAX_PATH)), productName);
    return std::make_tuple(std::move(result), nullptr);
}

[[nodiscard]] std::tuple<std::string, std::unique_ptr<Error>>
getResourceDirectoryPath() noexcept
{
    // NOTE: Get the path of the executable file.
    wchar_t path[MAX_PATH] = {};

    const auto length = ::GetModuleFileNameW(nullptr, path, MAX_PATH);
    if ((length == 0) || (length >= MAX_PATH)) {
        return std::make_tuple("", errors::make("GetModuleFileNameW() failed"));
    }

    auto directory = filepaths::getDirectoryName(toUTF8String(path, length));
    return std::make_tuple(std::move(directory), nullptr);
}

[[nodiscard]] std::tuple<std::string, std::unique_ptr<Error>>
getTempDirectoryPath() noexcept
{
    std::error_code err;
    auto path = std::filesystem::temp_directory_path(err);
    if (err) {
        return std::make_tuple("", errors::make("temp_directory_path() failed: " + err.message()));
    }
    return std::make_tuple(platformfs::toUTF8String(path), nullptr);
}

[[nodiscard]] std::tuple<std::string, std::unique_ptr<Error>>
getCurrentWorkingDirectory() noexcept
{
    wchar_t directory[MAX_PATH] = {};

    const auto length = ::GetCurrentDirectoryW(MAX_PATH, directory);
    if ((length == 0) || (length >= MAX_PATH)) {
        return std::make_tuple("", errors::make("GetCurrentDirectoryW() failed"));
    }
    return std::make_tuple(toUTF8String(directory, length), nullptr);
}

} // namespace pomdog::detail::win32
