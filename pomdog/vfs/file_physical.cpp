// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/vfs/file_physical.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/filesystem/file_system.h"
#include "pomdog/filesystem/filesystem_path.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/error_helper.h"
#include "pomdog/utility/path_helper.h"
#include "pomdog/vfs/file.h"
#include "pomdog/vfs/mount_volume.h"
#include "pomdog/vfs/not_found_error.h"
#include "pomdog/vfs/view_buffer.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <fcntl.h>
#include <sys/stat.h>
#include <cerrno>
#include <cstdio>
#include <string>
#include <string_view>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

#if defined(_MSC_VER)
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <io.h>
#include <share.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END
#else
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <unistd.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END
#endif

namespace pomdog::vfs {
namespace {

#if defined(_MSC_VER)
using StatBuffer = struct ::_stat64;
#else
using StatBuffer = struct ::stat;
#endif

/// Gets the status of the file at `path`, which is UTF-8. Returns 0 on success,
/// or the `errno` value on failure.
[[nodiscard]] int
statFile(const std::string& path, StatBuffer& st) noexcept
{
#if defined(_MSC_VER)
    if (::_wstat64(platformfs::toFilesystemPath(path).c_str(), &st) != 0) {
        return errno;
    }
#else
    if (::stat(path.c_str(), &st) != 0) {
        return errno;
    }
#endif
    return 0;
}

/// Returns true when `errnum` reports that a file or one of its parent directories does not exist.
[[nodiscard]] bool
isNotFoundErrno(int errnum) noexcept
{
    return (errnum == ENOENT) || (errnum == ENOTDIR);
}

/// FilePhysical is a file backed by a physical (OS) FILE*. It supports reading
/// and writing, and is suitable for regular files on disk.
class FilePhysical final : public File {
private:
    std::FILE* fp_ = nullptr;
    detail::ViewBuffer view_;
    bool writePending_ = false;
    bool writable_ = false;

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
        const bool canRead = mode.read;
        const bool canWrite = mode.write;
        if (!canRead && !canWrite) {
            return errors::make("file mode must allow reading or writing");
        }
        if (!canWrite && (mode.create || mode.append || mode.truncate)) {
            return errors::make("create, append, and truncate require write access");
        }

        int flags = 0;
        const char* streamMode = "rb";
        if (canRead && canWrite) {
            flags = O_RDWR;
            streamMode = mode.append ? "a+b" : "r+b";
        }
        else if (canWrite) {
            flags = O_WRONLY;
            streamMode = mode.append ? "ab" : "wb";
        }
        else {
            flags = O_RDONLY;
        }
        if (mode.create) {
            flags |= O_CREAT;
        }
        if (mode.append) {
            flags |= O_APPEND;
        }
        if (mode.truncate) {
            flags |= O_TRUNC;
        }

#if defined(_MSC_VER)
        int fd = -1;
        const int errnum = ::_wsopen_s(
            &fd,
            platformfs::toFilesystemPath(path).c_str(),
            flags | _O_BINARY,
            _SH_DENYNO,
            _S_IREAD | _S_IWRITE);
#else
        const int fd = ::open(path.c_str(), flags, 0666);
        const int errnum = (fd < 0) ? errno : 0;
#endif
        if (fd < 0) {
            if (isNotFoundErrno(errnum)) {
                return makeNotFoundError("file not found: " + path);
            }
            return errors::fromErrc(errors::toErrc(errnum), "failed to open file: " + path);
        }

        // NOTE: fdopen keeps the independently selected OS flags; unlike fopen,
        // its write mode does not create or truncate the file.
#if defined(_MSC_VER)
        fp_ = ::_fdopen(fd, streamMode);
#else
        fp_ = ::fdopen(fd, streamMode);
#endif
        if (fp_ == nullptr) {
            const auto streamErrno = errno;
#if defined(_MSC_VER)
            ::_close(fd);
#else
            ::close(fd);
#endif
            return errors::fromErrc(errors::toErrc(streamErrno), "failed to create file stream: " + path);
        }
        writable_ = canWrite;
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
        if (n > 0) {
            writePending_ = true;
        }
        if (std::ferror(fp_)) {
            return {n, errors::make("write error")};
        }
        return {n, nullptr};
    }

    [[nodiscard]] std::tuple<i64, std::unique_ptr<Error>>
    seek(i64 offset, SeekOrigin origin) noexcept override
    {
        POMDOG_ASSERT(fp_ != nullptr);

        // NOTE: Reject an origin outside the enum, as the archive files do, instead
        // of seeking from the beginning of the file.
        if ((origin != SeekOrigin::Begin) && (origin != SeekOrigin::Current) && (origin != SeekOrigin::End)) {
            return {0, errors::make("invalid seek origin")};
        }

        int whence = SEEK_SET;
        switch (origin) {
        case SeekOrigin::Begin:
            whence = SEEK_SET;
            break;
        case SeekOrigin::Current:
            whence = SEEK_CUR;
            break;
        case SeekOrigin::End:
            whence = SEEK_END;
            break;
        }

#if defined(_MSC_VER)
        if (::_fseeki64(fp_, offset, whence) != 0) {
            return {0, errors::make("seek failed")};
        }
        const auto pos = ::_ftelli64(fp_);
#else
        // NOTE: fseek() takes long, which is 32 bits on targets such as wasm32, so
        // reject an offset that the cast would truncate.
        if (!std::in_range<long>(offset)) {
            return {0, errors::make("seek offset out of range")};
        }
        if (std::fseek(fp_, static_cast<long>(offset), whence) != 0) {
            return {0, errors::make("seek failed")};
        }
        const auto pos = std::ftell(fp_);
#endif
        if (pos < 0) {
            return {0, errors::make("ftell failed")};
        }
        writePending_ = false;
        return {static_cast<i64>(pos), nullptr};
    }

    [[nodiscard]] std::unique_ptr<Error>
    close() noexcept override
    {
        view_.reset();

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

        // NOTE: fstat sees only the OS file size, not buffered stdio output.
        if (writePending_) {
            if (std::fflush(fp_) != 0) {
                return {0, errors::make("failed to flush file before getting its size")};
            }
            writePending_ = false;
        }

#if defined(_MSC_VER)
        int fd = ::_fileno(fp_);
#else
        int fd = ::fileno(fp_);
#endif
        if (fd < 0) {
            return {0, errors::make("fileno() failed")};
        }

        StatBuffer st = {};
#if defined(_MSC_VER)
        if (int res = ::_fstat64(fd, &st); res != 0) {
            return {0, errors::make("_fstat64() failed")};
        }
#else
        if (int res = ::fstat(fd, &st); res != 0) {
            return {0, errors::make("fstat() failed")};
        }
#endif
        return {static_cast<std::size_t>(st.st_size), nullptr};
    }

    [[nodiscard]] std::tuple<std::span<const u8>, std::unique_ptr<Error>>
    view() noexcept override
    {
        if (fp_ == nullptr) {
            return {std::span<const u8>{}, errors::make("file is closed")};
        }
        // NOTE: A view is a snapshot that later writes would make stale while it
        // stays valid, so only a file opened without write access has a view. Read
        // a writable file with read() instead.
        if (writable_) {
            return {std::span<const u8>{}, errors::make("cannot view a file opened with write access")};
        }
        if (view_.isAllocated()) {
            return {view_.bytes(), nullptr};
        }

        auto [size, sizeErr] = getSize();
        if (sizeErr != nullptr) {
            return {std::span<const u8>{}, std::move(sizeErr)};
        }

        if (auto err = view_.allocate(size); err != nullptr) {
            return {std::span<const u8>{}, std::move(err)};
        }

        // NOTE: Read the content from the beginning, and then restore the position
        // so that view() does not move it, even when the read fails.
        auto [position, tellErr] = seek(0, SeekOrigin::Current);
        if (tellErr != nullptr) {
            view_.reset();
            return {std::span<const u8>{}, std::move(tellErr)};
        }
        if (auto [begin, seekErr] = seek(0, SeekOrigin::Begin); seekErr != nullptr) {
            view_.reset();
            return {std::span<const u8>{}, std::move(seekErr)};
        }

        auto [bytesRead, readErr] = read(view_.bytes());
        if ((readErr == nullptr) && (bytesRead != size)) {
            readErr = errors::make("failed to read the whole file");
        }
        if (readErr != nullptr) {
            // NOTE: Clear the error and end-of-file indicators that the failed read
            // left, so that later reads and writes do not report them again.
            std::clearerr(fp_);
        }

        auto [restored, restoreErr] = seek(position, SeekOrigin::Begin);
        if (readErr != nullptr) {
            view_.reset();
            return {std::span<const u8>{}, std::move(readErr)};
        }
        if (restoreErr != nullptr) {
            view_.reset();
            return {std::span<const u8>{}, std::move(restoreErr)};
        }
        return {view_.bytes(), nullptr};
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

        StatBuffer st = {};
        if (const auto errnum = statFile(rootDir_, st); errnum != 0) {
            return errors::fromErrc(errors::toErrc(errnum), "failed to stat root directory: " + rootDir_);
        }

        if ((st.st_mode & S_IFDIR) == 0) {
            return errors::make("root path is not a directory: " + rootDir_);
        }
        return nullptr;
    }

    [[nodiscard]] std::tuple<std::shared_ptr<File>, std::unique_ptr<Error>>
    open(std::string_view path, const FileMode& mode) noexcept override
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
    exists(std::string_view path) noexcept override
    {
        const auto fullPath = pomdog::filepaths::join(rootDir_, path);
        return pomdog::platformfs::exists(fullPath);
    }

    [[nodiscard]] std::tuple<FileInfo, std::unique_ptr<Error>>
    stat(std::string_view path) noexcept override
    {
        const auto fullPath = pomdog::filepaths::join(rootDir_, path);

        StatBuffer st = {};
        if (const auto errnum = statFile(fullPath, st); errnum != 0) {
            if (isNotFoundErrno(errnum)) {
                return {FileInfo{}, makeNotFoundError("file not found: " + fullPath)};
            }
            return {FileInfo{}, errors::fromErrc(errors::toErrc(errnum), "failed to stat file: " + fullPath)};
        }

        FileInfo info = {};
        info.size = static_cast<std::size_t>(st.st_size);
        info.isDirectory = ((st.st_mode & S_IFDIR) != 0);
        return {std::move(info), nullptr};
    }

    [[nodiscard]] std::tuple<std::string, std::unique_ptr<Error>>
    toPhysicalPath(std::string_view path) noexcept override
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
