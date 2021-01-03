// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "Pomdog/Application/FileSystem.hpp"
#include "Pomdog/Basic/Platform.hpp"

#if defined(POMDOG_PLATFORM_MACOSX) || defined(POMDOG_PLATFORM_APPLE_IOS)
#include "../Platform.Apple/FileSystemApple.hpp"
#elif defined(POMDOG_PLATFORM_WIN32)
#include "../Platform.Win32/FileSystemWin32.hpp"
#elif defined(POMDOG_PLATFORM_LINUX) || defined(POMDOG_PLATFORM_ANDROID)
#include "../Platform.Linux/FileSystemLinux.hpp"
#else
#error "Platform undefined or not supported."
#endif

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Utility/PathHelper.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::FileSystem {
namespace {

#if defined(POMDOG_PLATFORM_MACOSX) || defined(POMDOG_PLATFORM_APPLE_IOS)
namespace PlatformFS = Detail::Apple;
#elif defined(POMDOG_PLATFORM_WIN32)
namespace PlatformFS = Detail::Win32;
#elif defined(POMDOG_PLATFORM_LINUX) || defined(POMDOG_PLATFORM_ANDROID)
namespace PlatformFS = Detail::Linux;
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
            return std::make_tuple("", Errors::Wrap(std::move(err), "GetCurrentWorkingDirectory() failed"));
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

} // namespace Pomdog::FileSystem
