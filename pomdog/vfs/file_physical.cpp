// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/vfs/file_physical.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/filesystem/file_system.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/path_helper.h"
#include "pomdog/vfs/file.h"
#include "pomdog/vfs/mount_volume.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <sys/stat.h>
#include <cstdio>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::vfs {
namespace {

/// FilePhysical is a file backed by a physical (OS) FILE*. It supports reading
/// and writing, and is suitable for regular files on disk.
class FilePhysical final : public File {
private:
    std::FILE* fp_ = nullptr;

public:
    FilePhysical() noexcept = default;

    ~FilePhysical() noexcept override
    {
        if (fp_ != nullptr) {
            std::fclose(fp_);
            fp_ = nullptr;
        }
    }

    [[nodiscard]] std::unique_ptr<Error>
    openFile(const std::string& path, const FileMode& mode) noexcept
    {
        std::string modeStr;
        const bool canRead = mode.read;
        const bool canWrite = mode.write;

        if (mode.create) {
            if (canRead && canWrite) {
                modeStr = mode.truncate ? "w+b" : "a+b";
            }
            else if (canWrite) {
                modeStr = mode.truncate ? "wb" : "ab";
            }
            else {
                return errors::make("cannot create a read-only file");
            }
        }
        else if (mode.append) {
            modeStr = canRead ? "a+b" : "ab";
        }
        else if (canRead && canWrite) {
            modeStr = "r+b";
        }
        else if (canWrite) {
            modeStr = "wb";
        }
        else {
            modeStr = "rb";
        }

#if defined(_MSC_VER)
        const auto err = fopen_s(&fp_, path.c_str(), modeStr.c_str());
        if (err != 0) {
            return errors::make("failed to open file: " + path);
        }
#else
        fp_ = std::fopen(path.c_str(), modeStr.c_str());
#endif
        if (fp_ == nullptr) {
            return errors::make("failed to open file: " + path);
        }

        return nullptr;
    }

    [[nodiscard]] std::tuple<std::size_t, std::unique_ptr<Error>>
    read(std::span<u8> buffer) noexcept override
    {
        POMDOG_ASSERT(fp_ != nullptr);
        const auto n = std::fread(buffer.data(), 1, buffer.size(), fp_);
        if (std::ferror(fp_)) {
            return {n, errors::make("read error")};
        }
        return {n, nullptr};
    }

    [[nodiscard]] std::tuple<std::size_t, std::unique_ptr<Error>>
    write(std::span<const u8> buffer) noexcept override
    {
        POMDOG_ASSERT(fp_ != nullptr);
        const auto n = std::fwrite(buffer.data(), 1, buffer.size(), fp_);
        if (std::ferror(fp_)) {
            return {n, errors::make("write error")};
        }
        return {n, nullptr};
    }

    [[nodiscard]] std::unique_ptr<Error>
    close() noexcept override
    {
        if (fp_ == nullptr) {
            return nullptr;
        }

        const auto result = std::fclose(fp_);
        fp_ = nullptr;

        if (result != 0) {
            return errors::make("failed to close file");
        }
        return nullptr;
    }

    [[nodiscard]] std::tuple<FileInfo, std::unique_ptr<Error>>
    stat() noexcept override
    {
        auto [size, err] = getSize();
        if (err != nullptr) {
            return {FileInfo{}, std::move(err)};
        }

        FileInfo info = {};
        info.size = size;
        info.isDirectory = false;
        return {std::move(info), nullptr};
    }

    [[nodiscard]] std::tuple<std::size_t, std::unique_ptr<Error>>
    getSize() noexcept override
    {
        POMDOG_ASSERT(fp_ != nullptr);

#if defined(_MSC_VER)
        int fd = ::_fileno(fp_);
#else
        int fd = ::fileno(fp_);
#endif
        if (fd < 0) {
            return {0, errors::make("fileno() failed")};
        }

        struct ::stat st = {};
        if (int res = ::fstat(fd, &st); res != 0) {
            return {0, errors::make("fstat() failed")};
        }
        return {static_cast<std::size_t>(st.st_size), nullptr};
    }
};

/// MountVolumePhysical implements the MountVolume interface for a physical directory on disk.
class MountVolumePhysical final : public MountVolume {
    using MountVolume::exists;
    using MountVolume::open;
    using MountVolume::stat;

private:
    std::string rootDir_;

public:
    [[nodiscard]] std::unique_ptr<Error>
    initialize(std::string&& rootDir) noexcept
    {
        rootDir_ = std::move(rootDir);

        struct ::stat st = {};
        if (auto res = ::stat(rootDir_.data(), &st); res != 0) {
            return errors::make("failed to stat root directory: " + rootDir_);
        }

        if ((st.st_mode & S_IFDIR) == 0) {
            return errors::make("root path is not a directory: " + rootDir_);
        }
        return nullptr;
    }

    [[nodiscard]] std::tuple<std::shared_ptr<File>, std::unique_ptr<Error>>
    open(const std::string& path, const FileMode& mode) noexcept override
    {
        const auto fullPath = pomdog::filepaths::join(rootDir_, path);

        auto file = std::make_shared<FilePhysical>();
        auto err = file->openFile(fullPath, mode);
        if (err != nullptr) {
            return {nullptr, std::move(err)};
        }

        return {std::move(file), nullptr};
    }

    [[nodiscard]] bool
    exists(const std::string& path) noexcept override
    {
        const auto fullPath = pomdog::filepaths::join(rootDir_, path);
        return pomdog::platformfs::exists(fullPath);
    }

    [[nodiscard]] std::tuple<FileInfo, std::unique_ptr<Error>>
    stat(const std::string& path) noexcept override
    {
        const auto fullPath = pomdog::filepaths::join(rootDir_, path);

        if (!pomdog::platformfs::exists(fullPath)) {
            return {FileInfo{}, errors::make("file not found: " + fullPath)};
        }

        auto [fileSize, sizeErr] = pomdog::platformfs::getFileSize(fullPath);
        if (sizeErr != nullptr) {
            return {FileInfo{}, errors::wrap(std::move(sizeErr), "failed to get file size: " + fullPath)};
        }

        FileInfo info;
        info.size = fileSize;
        info.isDirectory = pomdog::platformfs::isDirectory(fullPath);
        return {std::move(info), nullptr};
    }

    [[nodiscard]] std::tuple<std::string, std::unique_ptr<Error>>
    toPhysicalPath(const std::string& path) noexcept override
    {
        return {pomdog::filepaths::join(rootDir_, path), nullptr};
    }
};

} // namespace

std::tuple<std::shared_ptr<MountVolume>, std::unique_ptr<Error>>
createMountVolumePhysical(std::string rootDir) noexcept
{
    auto volume = std::make_shared<MountVolumePhysical>();
    if (auto err = volume->initialize(std::move(rootDir)); err != nullptr) {
        return {nullptr, errors::wrap(std::move(err), "failed to initialize physical mount volume")};
    }
    return {std::move(volume), nullptr};
}

} // namespace pomdog::vfs
