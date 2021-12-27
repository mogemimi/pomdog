// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/filesystem/file_system.h"
#include "pomdog/basic/platform.h"
#include "pomdog/utility/string_helper.h"
#include <catch_amalgamated.hpp>

using namespace pomdog;

TEST_CASE("FileSystem", "[FileSystem]")
{
    SECTION("IsDirectory")
    {
        REQUIRE(FileSystem::IsDirectory("."));
#if defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
        REQUIRE(FileSystem::IsDirectory("C:/"));
#else
        REQUIRE(FileSystem::IsDirectory("/"));
#endif
    }
    SECTION("Exists")
    {
        REQUIRE(FileSystem::IsDirectory("."));
#if defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
        REQUIRE(FileSystem::Exists("C:/"));
#else
        REQUIRE(FileSystem::Exists("/"));
#endif
    }
    SECTION("GetLocalAppDataDirectoryPath")
    {
#if defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
        auto [dir, err] = FileSystem::GetLocalAppDataDirectoryPath();
        REQUIRE(err == nullptr);
        REQUIRE(!dir.empty());
#elif defined(POMDOG_PLATFORM_MACOSX)
        auto [dir, err] = FileSystem::GetLocalAppDataDirectoryPath();
        if (err == nullptr) {
            REQUIRE(!dir.empty());
        }
#endif
    }
    SECTION("GetAppDataDirectoryPath")
    {
#if defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
        auto [dir, err] = FileSystem::GetAppDataDirectoryPath();
        REQUIRE(err == nullptr);
        REQUIRE(!dir.empty());
#elif defined(POMDOG_PLATFORM_MACOSX)
        auto [dir, err] = FileSystem::GetAppDataDirectoryPath();
        if (err == nullptr) {
            REQUIRE(!dir.empty());
        }
#endif
    }
    SECTION("GetResourceDirectoryPath")
    {
        auto [dir, err] = FileSystem::GetResourceDirectoryPath();
        REQUIRE(err == nullptr);
        REQUIRE(!dir.empty());
    }
    SECTION("GetTempDirectoryPath")
    {
        auto [dir, err] = FileSystem::GetTempDirectoryPath();
        REQUIRE(err == nullptr);
        REQUIRE(!dir.empty());
    }
    SECTION("GetCurrentWorkingDirectory")
    {
        auto [dir, err] = FileSystem::GetCurrentWorkingDirectory();
        REQUIRE(err == nullptr);
        REQUIRE(!dir.empty());
    }
    SECTION("GetFileSize")
    {
#if defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
        auto [size, err] = FileSystem::GetFileSize("C:\\Windows\\System32\\ntdll.dll");
        REQUIRE(err == nullptr);
        REQUIRE(size > 0);
#endif
    }
    SECTION("GetCanonicalPath")
    {
#if defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
        auto [dir, err] = FileSystem::GetCanonicalPath("./foo/./abc/../bar/");
        REQUIRE(err == nullptr);
        REQUIRE(StringHelper::HasSuffix(dir, "\\foo\\bar"));
#endif
    }
}
