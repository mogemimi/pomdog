// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/basic/platform.h"
#include "pomdog/filesystem/file_system.h"
#include "pomdog/utility/string_helper.h"
#include <catch_amalgamated.hpp>

using namespace pomdog;

TEST_CASE("FileSystem", "[FileSystem]")
{
    SECTION("isDirectory")
    {
        REQUIRE(FileSystem::isDirectory("."));
#if defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
        REQUIRE(FileSystem::isDirectory("C:/"));
#else
        REQUIRE(FileSystem::isDirectory("/"));
#endif
    }
    SECTION("exists")
    {
        REQUIRE(FileSystem::isDirectory("."));
#if defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
        REQUIRE(FileSystem::exists("C:/"));
#else
        REQUIRE(FileSystem::exists("/"));
#endif
    }
    SECTION("getLocalAppDataDirectoryPath")
    {
#if defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
        auto [dir, err] = FileSystem::getLocalAppDataDirectoryPath();
        REQUIRE(err == nullptr);
        REQUIRE(!dir.empty());
#elif defined(POMDOG_PLATFORM_MACOSX)
        auto [dir, err] = FileSystem::getLocalAppDataDirectoryPath();
        if (err == nullptr) {
            REQUIRE(!dir.empty());
        }
#endif
    }
    SECTION("getAppDataDirectoryPath")
    {
#if defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
        auto [dir, err] = FileSystem::getAppDataDirectoryPath();
        REQUIRE(err == nullptr);
        REQUIRE(!dir.empty());
#elif defined(POMDOG_PLATFORM_MACOSX)
        auto [dir, err] = FileSystem::getAppDataDirectoryPath();
        if (err == nullptr) {
            REQUIRE(!dir.empty());
        }
#endif
    }
    SECTION("getResourceDirectoryPath")
    {
        auto [dir, err] = FileSystem::getResourceDirectoryPath();
        REQUIRE(err == nullptr);
        REQUIRE(!dir.empty());
    }
    SECTION("getTempDirectoryPath")
    {
        auto [dir, err] = FileSystem::getTempDirectoryPath();
        REQUIRE(err == nullptr);
        REQUIRE(!dir.empty());
    }
    SECTION("getCurrentWorkingDirectory")
    {
        auto [dir, err] = FileSystem::getCurrentWorkingDirectory();
        REQUIRE(err == nullptr);
        REQUIRE(!dir.empty());
    }
    SECTION("getFileSize")
    {
#if defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
        auto [size, err] = FileSystem::getFileSize("C:\\Windows\\System32\\ntdll.dll");
        REQUIRE(err == nullptr);
        REQUIRE(size > 0);
#endif
    }
    SECTION("getCanonicalPath")
    {
#if defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
        auto [dir, err] = FileSystem::getCanonicalPath("./foo/./abc/../bar/");
        REQUIRE(err == nullptr);
        REQUIRE(strings::hasSuffix(dir, "\\foo\\bar"));
#endif
    }
}
