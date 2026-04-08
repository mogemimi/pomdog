// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/filesystem/file_system.h"
#include "pomdog/vfs/file.h"
#include "pomdog/vfs/file_system.h"
#include "pomdog/vfs/mount_volume.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdlib>
#include <string>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

using pomdog::u8;
using pomdog::vfs::File;
using pomdog::vfs::FileInfo;
using pomdog::vfs::FileMode;
using pomdog::vfs::FilePermissions;
using pomdog::vfs::FileSystemContext;
using pomdog::vfs::MountOptions;
using pomdog::vfs::MountVolume;

namespace {

/// Get the test data directory path from the POMDOG_TEST_DATA_DIR environment variable.
/// Falls back to the current working directory if not set.
[[nodiscard]] std::string
getTestDataDir() noexcept
{
#if defined(_MSC_VER)
    char* envVal = nullptr;
    std::size_t len = 0;
    if (_dupenv_s(&envVal, &len, "POMDOG_TEST_DATA_DIR") == 0 && envVal != nullptr) {
        std::string result(envVal);
        std::free(envVal);
        if (!result.empty()) {
            return result;
        }
    }
#else
    const char* envVal = std::getenv("POMDOG_TEST_DATA_DIR");
    if (envVal != nullptr && envVal[0] != '\0') {
        return std::string(envVal);
    }
#endif

    auto [cwd, err] = pomdog::platformfs::getCurrentWorkingDirectory();
    if (err == nullptr) {
        return cwd;
    }

    return ".";
}

} // namespace

TEST_CASE("VFS.Create")
{
    SUBCASE("create returns a valid context")
    {
        auto [ctx, err] = pomdog::vfs::create();
        REQUIRE(err == nullptr);
        REQUIRE(ctx.get() != nullptr);
    }
}

TEST_CASE("VFS.MountAndExists")
{
    auto testDir = getTestDataDir();

    auto [ctx, err] = pomdog::vfs::create();
    REQUIRE(err == nullptr);

    SUBCASE("mount a physical directory")
    {
        MountOptions options;
        options.readOnly = true;

        auto mountErr = pomdog::vfs::mount(ctx, "/data", testDir, options);
        REQUIRE(mountErr == nullptr);
    }

    SUBCASE("exists returns false for nonexistent file")
    {
        MountOptions options;
        options.readOnly = true;

        auto mountErr = pomdog::vfs::mount(ctx, "/data", testDir, options);
        REQUIRE(mountErr == nullptr);

        REQUIRE_FALSE(pomdog::vfs::exists(ctx, "/data/this_file_does_not_exist_12345.txt"));
    }

    SUBCASE("unmount")
    {
        MountOptions options;
        options.readOnly = true;

        auto mountErr = pomdog::vfs::mount(ctx, "/data", testDir, options);
        REQUIRE(mountErr == nullptr);

        auto unmountErr = pomdog::vfs::unmount(ctx, "/data");
        REQUIRE(unmountErr == nullptr);
    }

    SUBCASE("unmount nonexistent returns error")
    {
        auto unmountErr = pomdog::vfs::unmount(ctx, "/nonexistent");
        REQUIRE(unmountErr != nullptr);
    }
}

TEST_CASE("VFS.OpenAndRead")
{
    auto testDir = getTestDataDir();

    auto [ctx, err] = pomdog::vfs::create();
    REQUIRE(err == nullptr);

    MountOptions options;
    options.readOnly = true;

    auto mountErr = pomdog::vfs::mount(ctx, "/data", testDir, options);
    REQUIRE(mountErr == nullptr);

    SUBCASE("open nonexistent file returns error")
    {
        auto [file, openErr] = pomdog::vfs::open(ctx, "/data/nonexistent_file_12345.txt");
        REQUIRE(openErr != nullptr);
        REQUIRE(file.get() == nullptr);
    }
}

TEST_CASE("VFS.Resolve")
{
    auto testDir = getTestDataDir();

    auto [ctx, err] = pomdog::vfs::create();
    REQUIRE(err == nullptr);

    MountOptions options;
    options.readOnly = true;

    auto mountErr = pomdog::vfs::mount(ctx, "/data", testDir, options);
    REQUIRE(mountErr == nullptr);

    SUBCASE("resolve nonexistent returns error")
    {
        auto [resolved, resolveErr] = pomdog::vfs::resolve(ctx, "/data/nonexistent_file_12345.txt");
        REQUIRE(resolveErr != nullptr);
    }
}

TEST_CASE("VFS.OverlayFS")
{
    auto testDir = getTestDataDir();

    auto [ctx, err] = pomdog::vfs::create();
    REQUIRE(err == nullptr);

    // Mount two volumes at /data; the overlay should take priority.
    MountOptions baseOptions;
    baseOptions.readOnly = true;

    auto baseErr = pomdog::vfs::mount(ctx, "/data", testDir, baseOptions);
    REQUIRE(baseErr == nullptr);

    // Overlay mount with the same path
    MountOptions overlayOptions;
    overlayOptions.readOnly = true;
    overlayOptions.overlayFS = true;

    auto overlayErr = pomdog::vfs::mount(ctx, "/data", testDir, overlayOptions);
    REQUIRE(overlayErr == nullptr);

    // Both mounts are the same directory, so exists should still work.
    // This mainly tests that overlay insertion does not crash.
    REQUIRE_FALSE(pomdog::vfs::exists(ctx, "/data/nonexistent_overlay_file_12345.txt"));
}

TEST_CASE("VFS.WriteFile")
{
    auto [ctx, err] = pomdog::vfs::create();
    REQUIRE(err == nullptr);

    auto [tempDir, tempErr] = pomdog::platformfs::getTempDirectoryPath();
    if (tempErr != nullptr) {
        // Skip if we cannot get temp directory
        return;
    }

    MountOptions options;
    options.readOnly = false;

    auto mountErr = pomdog::vfs::mount(ctx, "/tmp", tempDir, options);
    REQUIRE(mountErr == nullptr);

    SUBCASE("write and read back")
    {
        FileMode writeMode;
        writeMode.write = true;
        writeMode.create = true;
        writeMode.truncate = true;

        auto [file, openErr] = pomdog::vfs::open(ctx, "/tmp/pomdog_vfs_test.tmp", writeMode);
        REQUIRE(openErr == nullptr);
        REQUIRE(file.get() != nullptr);

        const char content[] = "hello vfs";
        POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
        auto [written, writeErr] = file->write(std::span<const u8>{reinterpret_cast<const u8*>(content), sizeof(content) - 1});
        POMDOG_CLANG_UNSAFE_BUFFER_END
        REQUIRE(writeErr == nullptr);
        REQUIRE(written == sizeof(content) - 1);

        auto closeErr = file->close();
        REQUIRE(closeErr == nullptr);

        // Read it back
        auto [file2, openErr2] = pomdog::vfs::open(ctx, "/tmp/pomdog_vfs_test.tmp");
        REQUIRE(openErr2 == nullptr);
        REQUIRE(file2.get() != nullptr);

        char buf[64] = {};
        POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
        auto [readBytes, readErr] = file2->read(std::span<u8>{reinterpret_cast<u8*>(buf), sizeof(buf)});
        POMDOG_CLANG_UNSAFE_BUFFER_END
        REQUIRE(readErr == nullptr);
        REQUIRE(readBytes == sizeof(content) - 1);
        REQUIRE(std::string(buf, readBytes) == "hello vfs");

        auto closeErr2 = file2->close();
        REQUIRE(closeErr2 == nullptr);
    }
}
