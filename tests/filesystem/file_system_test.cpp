// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/filesystem/file_system.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

namespace platformfs = pomdog::platformfs;

TEST_CASE("FileSystem")
{
    SUBCASE("getCurrentWorkingDirectory")
    {
        auto [cwd, err] = platformfs::getCurrentWorkingDirectory();
        REQUIRE(err == nullptr);
        REQUIRE_FALSE(cwd.empty());
    }
    SUBCASE("exists")
    {
        auto [cwd, err] = platformfs::getCurrentWorkingDirectory();
        REQUIRE(err == nullptr);

        auto exists = platformfs::exists(cwd);
        REQUIRE(exists);
    }
    SUBCASE("isDirectory")
    {
        auto [cwd, err] = platformfs::getCurrentWorkingDirectory();
        REQUIRE(err == nullptr);

        auto isDir = platformfs::isDirectory(cwd);
        REQUIRE(isDir);
    }
    SUBCASE("exists(false)")
    {
        auto exists = platformfs::exists("/this/path/should/not/exist/12345");
        REQUIRE_FALSE(exists);
    }
    SUBCASE("getFileSize on directory")
    {
        auto [cwd, err] = platformfs::getCurrentWorkingDirectory();
        REQUIRE(err == nullptr);

        // NOTE: Directories may return 0 or a special value
        [[maybe_unused]] auto [size, sizeErr] = platformfs::getFileSize(cwd);
    }
    SUBCASE("getFileSize on existing file")
    {
#if defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
        auto [size, err] = platformfs::getFileSize("C:\\Windows\\System32\\ntdll.dll");
        REQUIRE(err == nullptr);
        REQUIRE(size > 0);
#endif
    }
    SUBCASE("getCanonicalPath")
    {
        auto [cwd, err] = platformfs::getCurrentWorkingDirectory();
        REQUIRE(err == nullptr);

        auto [canonical, canonicalErr] = platformfs::getCanonicalPath(cwd);
        REQUIRE(canonicalErr == nullptr);
        REQUIRE_FALSE(canonical.empty());
    }
    SUBCASE("getResourceDirectoryPath")
    {
        auto [dir, err] = platformfs::getResourceDirectoryPath();
        REQUIRE(err == nullptr);
        REQUIRE(!dir.empty());
    }
    SUBCASE("getTempDirectoryPath")
    {
        auto [dir, err] = platformfs::getTempDirectoryPath();
        REQUIRE(err == nullptr);
        REQUIRE(!dir.empty());
    }
    SUBCASE("getLocalAppDataDirectoryPath")
    {
#if defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
        auto [dir, err] = platformfs::getLocalAppDataDirectoryPath();
        if (err == nullptr) {
            REQUIRE(!dir.empty());
        }
#elif defined(POMDOG_PLATFORM_MACOSX)
        auto [dir, err] = platformfs::getLocalAppDataDirectoryPath();
        if (err == nullptr) {
            REQUIRE(!dir.empty());
        }
#endif
    }
    SUBCASE("getAppDataDirectoryPath")
    {
#if defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
        auto [dir, err] = platformfs::getAppDataDirectoryPath();
        if (err == nullptr) {
            REQUIRE(!dir.empty());
        }
#elif defined(POMDOG_PLATFORM_MACOSX)
        auto [dir, err] = platformfs::getAppDataDirectoryPath();
        if (err == nullptr) {
            REQUIRE(!dir.empty());
        }
#endif
    }
}
