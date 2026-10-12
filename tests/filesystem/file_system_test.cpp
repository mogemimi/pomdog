// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/filesystem/file_system.h"
#include "pomdog/filesystem/filesystem_path.h"
#include "pomdog/utility/path_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <system_error>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace platformfs = pomdog::platformfs;
using pomdog::platformfs::DirectoryCreation;

namespace {

/// Writes `text` to the file at `path`, which is UTF-8.
void writeTextFile(const std::string& path, std::string_view text)
{
    std::ofstream stream(platformfs::toFilesystemPath(path), std::ios::binary | std::ios::trunc);
    REQUIRE(stream.is_open());
    stream.write(text.data(), static_cast<std::streamsize>(text.size()));
    REQUIRE(stream.good());
}

} // namespace

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

TEST_CASE("FileSystemPath")
{
    // NOTE: The name falls outside the ANSI code page of most Windows systems.
    const std::string name = "pomdog_platformfs_テスト_ü";

    SUBCASE("converts UTF-8 to a path and back")
    {
        const auto path = platformfs::toFilesystemPath(name);
        const auto utf8 = platformfs::toUTF8String(path);
        REQUIRE(utf8 == name);
        const auto empty = platformfs::toUTF8String(platformfs::toFilesystemPath(""));
        REQUIRE(empty.empty());
    }
    SUBCASE("functions take UTF-8 paths")
    {
        auto [tempDir, tempErr] = platformfs::getTempDirectoryPath();
        REQUIRE(tempErr == nullptr);

        const auto directory = pomdog::filepaths::join(tempDir, name);
        const auto filePath = pomdog::filepaths::join(directory, "file.txt");

        std::error_code removeErr;
        std::filesystem::remove_all(platformfs::toFilesystemPath(directory), removeErr);

        auto [creation, createErr] = platformfs::createDirectories(directory);
        REQUIRE(createErr == nullptr);
        REQUIRE(creation == DirectoryCreation::Created);
        REQUIRE(platformfs::exists(directory));
        REQUIRE(platformfs::isDirectory(directory));

        {
            std::ofstream stream(platformfs::toFilesystemPath(filePath), std::ios::binary);
            REQUIRE(stream.is_open());
            stream << "12345";
        }
        REQUIRE(platformfs::exists(filePath));
        REQUIRE_FALSE(platformfs::isDirectory(filePath));

        auto [size, sizeErr] = platformfs::getFileSize(filePath);
        REQUIRE(sizeErr == nullptr);
        REQUIRE(size == 5);

        std::filesystem::remove_all(platformfs::toFilesystemPath(directory), removeErr);
        REQUIRE_FALSE(removeErr);
        REQUIRE_FALSE(platformfs::exists(directory));
    }
}

TEST_CASE("FileSystemDirectories")
{
    auto [tempDir, tempErr] = platformfs::getTempDirectoryPath();
    REQUIRE(tempErr == nullptr);

    const auto root = pomdog::filepaths::join(tempDir, "pomdog_platformfs_directories_test");
    std::error_code removeErr;
    std::filesystem::remove_all(platformfs::toFilesystemPath(root), removeErr);

    SUBCASE("createDirectory reports whether it created the directory")
    {
        auto [creation, err] = platformfs::createDirectory(root);
        REQUIRE(err == nullptr);
        REQUIRE(creation == DirectoryCreation::Created);
        REQUIRE(platformfs::isDirectory(root));

        auto [again, againErr] = platformfs::createDirectory(root);
        REQUIRE(againErr == nullptr);
        REQUIRE(again == DirectoryCreation::AlreadyExists);
    }
    SUBCASE("createDirectory fails without the parent directory")
    {
        auto [creation, err] = platformfs::createDirectory(pomdog::filepaths::join(root, "missing/child"));
        REQUIRE(err != nullptr);
        REQUIRE_FALSE(platformfs::exists(root));
    }
    SUBCASE("createDirectories reports whether it created the directory")
    {
        const auto nested = pomdog::filepaths::join(root, "a/b/c");
        auto [creation, err] = platformfs::createDirectories(nested);
        REQUIRE(err == nullptr);
        REQUIRE(creation == DirectoryCreation::Created);
        REQUIRE(platformfs::isDirectory(nested));

        auto [again, againErr] = platformfs::createDirectories(nested);
        REQUIRE(againErr == nullptr);
        REQUIRE(again == DirectoryCreation::AlreadyExists);

        auto [trailing, trailingErr] = platformfs::createDirectories(nested + "/");
        REQUIRE(trailingErr == nullptr);
        REQUIRE(trailing == DirectoryCreation::AlreadyExists);

        auto [parent, parentErr] = platformfs::createDirectories(pomdog::filepaths::join(root, "a"));
        REQUIRE(parentErr == nullptr);
        REQUIRE(parent == DirectoryCreation::AlreadyExists);
    }
    SUBCASE("a file in place of a directory is an error")
    {
        auto [rootCreation, rootErr] = platformfs::createDirectories(root);
        REQUIRE(rootErr == nullptr);
        REQUIRE(rootCreation == DirectoryCreation::Created);
        const auto filePath = pomdog::filepaths::join(root, "file.txt");
        writeTextFile(filePath, "file");

        auto [directory, directoryErr] = platformfs::createDirectory(filePath);
        REQUIRE(directoryErr != nullptr);

        auto [directories, directoriesErr] = platformfs::createDirectories(filePath);
        REQUIRE(directoriesErr != nullptr);

        auto [child, childErr] = platformfs::createDirectories(pomdog::filepaths::join(filePath, "child"));
        REQUIRE(childErr != nullptr);

        REQUIRE_FALSE(platformfs::isDirectory(filePath));
    }

    std::filesystem::remove_all(platformfs::toFilesystemPath(root), removeErr);
}
