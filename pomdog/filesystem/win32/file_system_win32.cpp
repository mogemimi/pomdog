// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/filesystem/win32/file_system_win32.h"
#include "pomdog/platform/win32/prerequisites_win32.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/error_helper.h"
#include "pomdog/utility/path_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <Shlobj.h>
#include <filesystem>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::win32 {

[[nodiscard]] std::unique_ptr<Error>
createNewDirectory(const std::string& path) noexcept
{
    POMDOG_ASSERT(!path.empty());
    std::error_code err;
    if (!std::filesystem::create_directory(path, err)) {
        return errors::make("create_directory() failed: " + err.message());
    }
    return nullptr;
}

[[nodiscard]] std::unique_ptr<Error>
createDirectories(const std::string& path) noexcept
{
    POMDOG_ASSERT(!path.empty());
    std::error_code err;
    if (!std::filesystem::create_directories(path, err)) {
        return errors::make("create_directories() failed: " + err.message());
    }
    return nullptr;
}

[[nodiscard]] bool
exists(const std::string& path) noexcept
{
    POMDOG_ASSERT(!path.empty());
    return std::filesystem::exists(path);
}

[[nodiscard]] bool
isDirectory(const std::string& path) noexcept
{
    POMDOG_ASSERT(!path.empty());
    return std::filesystem::is_directory(path);
}

[[nodiscard]] std::tuple<std::size_t, std::unique_ptr<Error>>
getFileSize(const std::string& path) noexcept
{
    struct ::stat st;
    if (int result = ::stat(path.data(), &st); result != 0) {
        const auto err = detail::toErrc(errno);
        return std::make_tuple(0, errors::makeIOError(err, "::stat() failed"));
    }
    return std::make_tuple(st.st_size, nullptr);
}

[[nodiscard]] std::tuple<std::string, std::unique_ptr<Error>>
getLocalAppDataDirectoryPath() noexcept
{
    char directory[MAX_PATH];
    directory[MAX_PATH - 1] = '\0';

    auto hr = SHGetFolderPathA(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, directory);
    if (FAILED(hr)) {
        return std::make_tuple("", errors::make("SHGetFolderPath(CSIDL_LOCAL_APPDATA) failed: " + std::to_string(hr)));
    }

    // FIXME: Change so that the user can specify any application name.
    constexpr auto productName = "pomdog";

    auto result = filepaths::join(directory, productName);
    return std::make_tuple(std::move(result), nullptr);
}

[[nodiscard]] std::tuple<std::string, std::unique_ptr<Error>>
getAppDataDirectoryPath() noexcept
{
    char directory[MAX_PATH];
    directory[MAX_PATH - 1] = '\0';

    auto hr = SHGetFolderPathA(nullptr, CSIDL_APPDATA, nullptr, 0, directory);
    if (FAILED(hr)) {
        return std::make_tuple("", errors::make("SHGetFolderPath(CSIDL_APPDATA) failed: " + std::to_string(hr)));
    }

    // FIXME: Change so that the user can specify any application name.
    constexpr auto productName = "pomdog";

    auto result = filepaths::join(directory, productName);
    return std::make_tuple(std::move(result), nullptr);
}

[[nodiscard]] std::tuple<std::string, std::unique_ptr<Error>>
getResourceDirectoryPath() noexcept
{
    auto currentDirectory = std::filesystem::current_path();
    return std::make_tuple(currentDirectory.string(), nullptr);
}

[[nodiscard]] std::tuple<std::string, std::unique_ptr<Error>>
getTempDirectoryPath() noexcept
{
    auto path = std::filesystem::temp_directory_path();
    return std::make_tuple(path.string(), nullptr);
}

[[nodiscard]] std::tuple<std::string, std::unique_ptr<Error>>
getCurrentWorkingDirectory() noexcept
{
    char directory[MAX_PATH];
    directory[MAX_PATH - 1] = '\0';

    const auto length = ::GetCurrentDirectoryA(MAX_PATH - 1, directory);
    if (length <= 0) {
        return std::make_tuple("", errors::make("GetCurrentDirectoryA() failed"));
    }
    std::string result(directory, length);
    return std::make_tuple(std::move(result), nullptr);
}

} // namespace pomdog::detail::win32
