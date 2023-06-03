// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/filesystem/file_system.h"
#include "pomdog/basic/platform.h"

#if defined(POMDOG_PLATFORM_MACOSX) || defined(POMDOG_PLATFORM_APPLE_IOS)
#include "pomdog/filesystem/apple/file_system_apple.h"
#elif defined(POMDOG_PLATFORM_WIN32)
#include "pomdog/filesystem/win32/file_system_win32.h"
#elif defined(POMDOG_PLATFORM_LINUX) || defined(POMDOG_PLATFORM_ANDROID)
#include "pomdog/filesystem/linux/file_system_linux.h"
#elif defined(POMDOG_PLATFORM_EMSCRIPTEN)
#include "pomdog/filesystem/emscripten/file_system_emscripten.h"
#else
#error "Platform undefined or not supported."
#endif

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/utility/path_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::FileSystem {
namespace {

#if defined(POMDOG_PLATFORM_MACOSX) || defined(POMDOG_PLATFORM_APPLE_IOS)
namespace PlatformFS = detail::apple;
#elif defined(POMDOG_PLATFORM_WIN32)
namespace PlatformFS = detail::win32;
#elif defined(POMDOG_PLATFORM_LINUX) || defined(POMDOG_PLATFORM_ANDROID)
namespace PlatformFS = detail::linux;
#elif defined(POMDOG_PLATFORM_EMSCRIPTEN)
namespace PlatformFS = detail::emscripten;
#else
#error "Platform undefined or not supported."
#endif

} // namespace

[[nodiscard]] std::unique_ptr<Error>
createNewDirectory(const std::string& path) noexcept
{
    return PlatformFS::createNewDirectory(path);
}

[[nodiscard]] std::unique_ptr<Error>
createDirectories(const std::string& path) noexcept
{
    return PlatformFS::createDirectories(path);
}

[[nodiscard]] bool
exists(const std::string& path) noexcept
{
    return PlatformFS::exists(path);
}

[[nodiscard]] bool
isDirectory(const std::string& path) noexcept
{
    return PlatformFS::isDirectory(path);
}

[[nodiscard]] std::tuple<std::size_t, std::unique_ptr<Error>>
getFileSize(const std::string& path) noexcept
{
    return PlatformFS::getFileSize(path);
}

[[nodiscard]] std::tuple<std::string, std::unique_ptr<Error>>
getCanonicalPath(const std::string& path) noexcept
{
    auto result = filepaths::normalize(path);

    if (!filepaths::isAbsolute(result)) {
        auto [cwd, err] = FileSystem::getCurrentWorkingDirectory();
        if (err != nullptr) {
            return std::make_tuple("", errors::wrap(std::move(err), "GetCurrentWorkingDirectory() failed"));
        }
        result = filepaths::normalize(filepaths::join(cwd, result));
    }
    return std::make_tuple(std::move(result), nullptr);
}

[[nodiscard]] std::tuple<std::string, std::unique_ptr<Error>>
getLocalAppDataDirectoryPath() noexcept
{
    return PlatformFS::getLocalAppDataDirectoryPath();
}

[[nodiscard]] std::tuple<std::string, std::unique_ptr<Error>>
getAppDataDirectoryPath() noexcept
{
    return PlatformFS::getAppDataDirectoryPath();
}

[[nodiscard]] std::tuple<std::string, std::unique_ptr<Error>>
getResourceDirectoryPath() noexcept
{
    return PlatformFS::getResourceDirectoryPath();
}

[[nodiscard]] std::tuple<std::string, std::unique_ptr<Error>>
getTempDirectoryPath() noexcept
{
    return PlatformFS::getTempDirectoryPath();
}

[[nodiscard]] std::tuple<std::string, std::unique_ptr<Error>>
getCurrentWorkingDirectory() noexcept
{
    return PlatformFS::getCurrentWorkingDirectory();
}

} // namespace pomdog::FileSystem
