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

std::unique_ptr<Error>
CreateNewDirectory(const std::string& path) noexcept
{
    return PlatformFS::CreateNewDirectory(path);
}

std::unique_ptr<Error>
CreateDirectories(const std::string& path) noexcept
{
    return PlatformFS::CreateDirectories(path);
}

bool Exists(const std::string& path) noexcept
{
    return PlatformFS::Exists(path);
}

bool IsDirectory(const std::string& path) noexcept
{
    return PlatformFS::IsDirectory(path);
}

std::tuple<std::size_t, std::unique_ptr<Error>>
GetFileSize(const std::string& path) noexcept
{
    return PlatformFS::GetFileSize(path);
}

std::tuple<std::string, std::unique_ptr<Error>>
GetCanonicalPath(const std::string& path) noexcept
{
    auto result = PathHelper::Normalize(path);

    if (!PathHelper::IsAbsolute(result)) {
        auto [cwd, err] = FileSystem::GetCurrentWorkingDirectory();
        if (err != nullptr) {
            return std::make_tuple("", errors::Wrap(std::move(err), "GetCurrentWorkingDirectory() failed"));
        }
        result = PathHelper::Normalize(PathHelper::Join(cwd, result));
    }
    return std::make_tuple(std::move(result), nullptr);
}

std::tuple<std::string, std::unique_ptr<Error>>
GetLocalAppDataDirectoryPath() noexcept
{
    return PlatformFS::GetLocalAppDataDirectoryPath();
}

std::tuple<std::string, std::unique_ptr<Error>>
GetAppDataDirectoryPath() noexcept
{
    return PlatformFS::GetAppDataDirectoryPath();
}

std::tuple<std::string, std::unique_ptr<Error>>
GetResourceDirectoryPath() noexcept
{
    return PlatformFS::GetResourceDirectoryPath();
}

std::tuple<std::string, std::unique_ptr<Error>>
GetTempDirectoryPath() noexcept
{
    return PlatformFS::GetTempDirectoryPath();
}

std::tuple<std::string, std::unique_ptr<Error>>
GetCurrentWorkingDirectory() noexcept
{
    return PlatformFS::GetCurrentWorkingDirectory();
}

} // namespace pomdog::FileSystem
