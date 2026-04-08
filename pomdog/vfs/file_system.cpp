// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/vfs/file_system.h"

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/filesystem/file_system.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/string_hash64.h"
#include "pomdog/vfs/file_physical.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <mutex>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::vfs {
namespace {

/// MountEntry represents a single mount point in the VFS. It contains the virtual path
/// where the volume is mounted, a shared pointer to the MountVolume, and the mount options
/// that specify how the volume should be accessed (e.g. read-only, overlay).
struct MountEntry final {
    std::string virtualPath;
    std::shared_ptr<MountVolume> volume;
    MountOptions options;
    u64 virtualPathHash = 0;
};

/// ResolveFlags are used internally to specify the type of resolution being performed
/// when looking up files in the mounted volumes. They indicate whether we are checking for existence,
/// reading, or writing.
enum class ResolveFlags : u8 {
    Exist,
    Read,
    Write,
};

/// Returns the relative path within the mount for the given virtual path as a string view.
/// If the virtual path does not start with the mount prefix, returns an empty string view.
[[nodiscard]] std::string_view
getRelativePathAsView(std::string_view mountPath, std::string_view virtualPath) noexcept
{
    if (mountPath.empty() || mountPath == "/") {
        // NOTE: Root mount: everything is relative.
        if (!virtualPath.empty() && virtualPath[0] == '/') {
            return virtualPath.substr(1);
        }
        return virtualPath;
    }

    if (virtualPath == mountPath) {
        return "";
    }

    if (virtualPath.size() > mountPath.size() &&
        virtualPath.compare(0, mountPath.size(), mountPath) == 0 &&
        virtualPath[mountPath.size()] == '/') {
        return virtualPath.substr(mountPath.size() + 1);
    }

    return "";
}

/// Returns the relative path within the mount for the given virtual path.
/// If the virtual path does not start with the mount prefix, returns an empty string.
[[nodiscard]] std::string
getRelativePath(std::string_view mountPath, std::string_view virtualPath) noexcept
{
    return std::string{getRelativePathAsView(mountPath, virtualPath)};
}

} // namespace

/// FileSystemContext manages the list of mounted volumes and implements the logic for resolving
/// file paths, opening files, and performing file operations. Each method acquires an internal
/// mutex, so it is safe to call from multiple threads concurrently.
class FileSystemContext final {
private:
    std::vector<MountEntry> mounts_;
    std::mutex mutex_;

public:
    FileSystemContext() noexcept = default;
    ~FileSystemContext() noexcept = default;

    FileSystemContext(const FileSystemContext&) = delete;
    FileSystemContext& operator=(const FileSystemContext&) = delete;

    [[nodiscard]] std::unique_ptr<Error>
    addMount(
        const std::string& virtualPath,
        const std::string& physicalPath,
        const MountOptions& options) noexcept
    {
        auto [volume, err] = createMountVolumePhysical(std::string(physicalPath));
        if (err != nullptr) {
            return errors::wrap(std::move(err), "failed to create physical mount volume");
        }
        return addMountVolume(virtualPath, std::move(volume), options);
    }

    [[nodiscard]] std::unique_ptr<Error>
    addMountVolume(
        const std::string& virtualPath,
        std::shared_ptr<MountVolume>&& volume,
        const MountOptions& options) noexcept
    {
        if (volume == nullptr) {
            return errors::make("mount volume is null");
        }
        if (virtualPath.empty()) {
            return errors::make("virtual path cannot be empty");
        }
        if (!virtualPath.starts_with('/')) {
            return errors::make("virtual path must start with '/': " + virtualPath);
        }
        if (options.overlayFS && !options.readOnly) {
            return errors::make("overlayFS volumes must be read-only");
        }
        if (options.overlayFS && options.persistentDataStorage) {
            return errors::make("overlayFS volumes cannot be used for persistent data storage");
        }

        MountEntry entry;
        entry.virtualPath = virtualPath;
        entry.volume = std::move(volume);
        entry.options = options;
        entry.virtualPathHash = computeStringHash64(virtualPath);

        std::lock_guard<std::mutex> lock(mutex_);

        if (!options.overlayFS) {
            auto it = std::find_if(mounts_.begin(), mounts_.end(),
                [&](const MountEntry& e) { return e.virtualPath == virtualPath; });

            if (it != mounts_.end()) {
                return errors::make("mount point conflicts with existing mount: " + virtualPath);
            }
        }

        if (options.overlayFS) {
            // NOTE: Overlay FS entries are inserted at the front so they take priority.
            mounts_.insert(mounts_.begin(), std::move(entry));
        }
        else {
            mounts_.push_back(std::move(entry));
        }

        return nullptr;
    }

    [[nodiscard]] std::unique_ptr<Error>
    removeMount(const std::string& virtualPath) noexcept
    {
        std::lock_guard<std::mutex> lock(mutex_);

        auto it = std::find_if(mounts_.begin(), mounts_.end(),
            [&](const MountEntry& e) { return e.virtualPath == virtualPath; });

        if (it == mounts_.end()) {
            return errors::make("mount point not found: " + virtualPath);
        }

        mounts_.erase(it);
        return nullptr;
    }

    [[nodiscard]] std::tuple<std::shared_ptr<File>, std::unique_ptr<Error>>
    openFile(const std::string& path, const FileMode& mode) noexcept
    {
        std::lock_guard<std::mutex> lock(mutex_);

        const bool needWrite = mode.write;
        const auto flags = needWrite ? ResolveFlags::Write : ResolveFlags::Read;

        for (auto& mount : mounts_) {
            auto relativePath = getRelativePath(mount.virtualPath, path);
            if (relativePath.empty() && path != mount.virtualPath) {
                continue;
            }

            if (needWrite && mount.options.readOnly) {
                continue;
            }

            if (flags == ResolveFlags::Read && !mount.volume->exists(relativePath)) {
                continue;
            }

            auto [file, err] = mount.volume->open(relativePath, mode);
            if (err != nullptr) {
                continue;
            }
            return {std::move(file), nullptr};
        }

        return {nullptr, errors::make("file not found: " + path)};
    }

    [[nodiscard]] std::tuple<FileInfo, std::unique_ptr<Error>>
    statFile(const std::string& path) noexcept
    {
        std::lock_guard<std::mutex> lock(mutex_);

        for (auto& mount : mounts_) {
            auto relativePath = getRelativePath(mount.virtualPath, path);
            if (relativePath.empty() && path != mount.virtualPath) {
                continue;
            }

            if (!mount.volume->exists(relativePath)) {
                continue;
            }

            auto [info, err] = mount.volume->stat(relativePath);
            if (err == nullptr) {
                return {std::move(info), nullptr};
            }
        }

        return {FileInfo{}, errors::make("file not found: " + path)};
    }

    [[nodiscard]] bool
    existsFile(const std::string& path) noexcept
    {
        std::lock_guard<std::mutex> lock(mutex_);

        for (auto& mount : mounts_) {
            auto relativePath = getRelativePath(mount.virtualPath, path);
            if (relativePath.empty() && path != mount.virtualPath) {
                continue;
            }

            if (mount.volume->exists(relativePath)) {
                return true;
            }
        }

        return false;
    }

    [[nodiscard]] std::tuple<std::string, std::unique_ptr<Error>>
    resolveFile(const std::string& path) noexcept
    {
        std::lock_guard<std::mutex> lock(mutex_);

        for (auto& mount : mounts_) {
            auto relativePath = getRelativePath(mount.virtualPath, path);
            if (relativePath.empty() && path != mount.virtualPath) {
                continue;
            }

            if (!mount.volume->exists(relativePath)) {
                continue;
            }

            auto [physPath, err] = mount.volume->toPhysicalPath(relativePath);
            if (err == nullptr) {
                return {std::move(physPath), nullptr};
            }
        }

        return {"", errors::make("cannot resolve path: " + path)};
    }

    [[nodiscard]] std::unique_ptr<Error>
    mkdir(const std::string& path) noexcept
    {
        std::lock_guard<std::mutex> lock(mutex_);

        for (auto& mount : mounts_) {
            if (mount.options.readOnly) {
                continue;
            }

            auto relativePath = getRelativePath(mount.virtualPath, path);
            if (relativePath.empty() && path != mount.virtualPath) {
                continue;
            }

            auto [physPath, err] = mount.volume->toPhysicalPath(relativePath);
            if (err != nullptr) {
                continue;
            }

            return pomdog::platformfs::createNewDirectory(physPath);
        }

        return errors::make("no writable mount for: " + path);
    }

    [[nodiscard]] std::unique_ptr<Error>
    mkdirAll(const std::string& path) noexcept
    {
        std::lock_guard<std::mutex> lock(mutex_);

        for (auto& mount : mounts_) {
            if (mount.options.readOnly) {
                continue;
            }

            auto relativePath = getRelativePath(mount.virtualPath, path);
            if (relativePath.empty() && path != mount.virtualPath) {
                continue;
            }

            auto [physPath, err] = mount.volume->toPhysicalPath(relativePath);
            if (err != nullptr) {
                continue;
            }

            return pomdog::platformfs::createDirectories(physPath);
        }

        return errors::make("no writable mount for: " + path);
    }

    [[nodiscard]] std::tuple<std::shared_ptr<File>, std::unique_ptr<Error>>
    openFileByHash(u64 mountVolumeHashKey, u64 fileHashKey, const FileMode& mode) noexcept
    {
        std::lock_guard<std::mutex> lock(mutex_);

        for (auto& mount : mounts_) {
            if (mode.write && mount.options.readOnly) {
                continue;
            }
            if (!mount.options.hashKeyLookup) {
                continue;
            }
            if (mount.virtualPathHash != mountVolumeHashKey) {
                continue;
            }

            auto [file, err] = mount.volume->open(fileHashKey, mode);
            if (err == nullptr) {
                return {std::move(file), nullptr};
            }
        }

        return {nullptr, errors::make("file not found for hash key")};
    }

    [[nodiscard]] std::tuple<FileInfo, std::unique_ptr<Error>>
    statFileByHash(u64 mountVolumeHashKey, u64 fileHashKey) noexcept
    {
        std::lock_guard<std::mutex> lock(mutex_);

        for (auto& mount : mounts_) {
            if (!mount.options.hashKeyLookup) {
                continue;
            }
            if (mount.virtualPathHash != mountVolumeHashKey) {
                continue;
            }
            if (mount.volume->exists(fileHashKey)) {
                auto [info, err] = mount.volume->stat(fileHashKey);
                if (err == nullptr) {
                    return {std::move(info), nullptr};
                }
            }
        }

        return {FileInfo{}, errors::make("file not found for hash key")};
    }

    [[nodiscard]] bool
    existsFileByHash(u64 mountVolumeHashKey, u64 fileHashKey) noexcept
    {
        std::lock_guard<std::mutex> lock(mutex_);

        for (auto& mount : mounts_) {
            if (!mount.options.hashKeyLookup) {
                continue;
            }
            if (mount.virtualPathHash != mountVolumeHashKey) {
                continue;
            }
            if (mount.volume->exists(fileHashKey)) {
                return true;
            }
        }

        return false;
    }
};

[[nodiscard]] std::tuple<std::shared_ptr<FileSystemContext>, std::unique_ptr<Error>>
create() noexcept
{
    auto ctx = std::make_shared<FileSystemContext>();
    return {std::move(ctx), nullptr};
}

[[nodiscard]] std::unique_ptr<Error>
mount(
    const std::shared_ptr<FileSystemContext>& ctx,
    const std::string& virtualPath,
    const std::string& physicalPath,
    const MountOptions& options) noexcept
{
    POMDOG_ASSERT(ctx != nullptr);
    return ctx->addMount(virtualPath, physicalPath, options);
}

[[nodiscard]] std::unique_ptr<Error>
mount(
    const std::shared_ptr<FileSystemContext>& ctx,
    const std::string& virtualPath,
    std::shared_ptr<MountVolume>&& volume,
    const MountOptions& options) noexcept
{
    POMDOG_ASSERT(ctx != nullptr);
    return ctx->addMountVolume(virtualPath, std::move(volume), options);
}

[[nodiscard]] std::unique_ptr<Error>
unmount(
    const std::shared_ptr<FileSystemContext>& ctx,
    const std::string& virtualPath) noexcept
{
    POMDOG_ASSERT(ctx != nullptr);
    return ctx->removeMount(virtualPath);
}

[[nodiscard]] std::unique_ptr<Error>
mkdir(
    const std::shared_ptr<FileSystemContext>& ctx,
    const std::string& path) noexcept
{
    POMDOG_ASSERT(ctx != nullptr);
    return ctx->mkdir(path);
}

[[nodiscard]] std::unique_ptr<Error>
mkdirAll(
    const std::shared_ptr<FileSystemContext>& ctx,
    const std::string& path) noexcept
{
    POMDOG_ASSERT(ctx != nullptr);
    return ctx->mkdirAll(path);
}

[[nodiscard]] std::tuple<std::shared_ptr<File>, std::unique_ptr<Error>>
open(
    const std::shared_ptr<FileSystemContext>& ctx,
    const std::string& path,
    const FileMode& mode) noexcept
{
    POMDOG_ASSERT(ctx != nullptr);
    return ctx->openFile(path, mode);
}

[[nodiscard]] std::tuple<std::shared_ptr<File>, std::unique_ptr<Error>>
open(
    const std::shared_ptr<FileSystemContext>& ctx,
    const std::string& path) noexcept
{
    auto mode = FileMode{
        .read = true,
    };
    return open(ctx, path, mode);
}

[[nodiscard]] std::tuple<FileInfo, std::unique_ptr<Error>>
stat(
    const std::shared_ptr<FileSystemContext>& ctx,
    const std::string& path) noexcept
{
    POMDOG_ASSERT(ctx != nullptr);
    return ctx->statFile(path);
}

[[nodiscard]] bool
exists(
    const std::shared_ptr<FileSystemContext>& ctx,
    const std::string& path) noexcept
{
    POMDOG_ASSERT(ctx != nullptr);
    return ctx->existsFile(path);
}

[[nodiscard]] std::tuple<std::string, std::unique_ptr<Error>>
resolve(
    const std::shared_ptr<FileSystemContext>& ctx,
    const std::string& path) noexcept
{
    POMDOG_ASSERT(ctx != nullptr);
    return ctx->resolveFile(path);
}

[[nodiscard]] std::tuple<std::shared_ptr<File>, std::unique_ptr<Error>>
open(
    const std::shared_ptr<FileSystemContext>& ctx,
    u64 mountVolumeHashKey,
    u64 fileHashKey,
    const FileMode& mode) noexcept
{
    POMDOG_ASSERT(ctx != nullptr);
    return ctx->openFileByHash(mountVolumeHashKey, fileHashKey, mode);
}

[[nodiscard]] std::tuple<std::shared_ptr<File>, std::unique_ptr<Error>>
open(
    const std::shared_ptr<FileSystemContext>& ctx,
    u64 mountVolumeHashKey,
    u64 fileHashKey) noexcept
{
    auto mode = FileMode{
        .read = true,
    };
    return open(ctx, mountVolumeHashKey, fileHashKey, mode);
}

[[nodiscard]] std::tuple<FileInfo, std::unique_ptr<Error>>
stat(
    const std::shared_ptr<FileSystemContext>& ctx,
    u64 mountVolumeHashKey,
    u64 fileHashKey) noexcept
{
    POMDOG_ASSERT(ctx != nullptr);
    return ctx->statFileByHash(mountVolumeHashKey, fileHashKey);
}

[[nodiscard]] bool
exists(
    const std::shared_ptr<FileSystemContext>& ctx,
    u64 mountVolumeHashKey,
    u64 fileHashKey) noexcept
{
    POMDOG_ASSERT(ctx != nullptr);
    return ctx->existsFileByHash(mountVolumeHashKey, fileHashKey);
}

} // namespace pomdog::vfs
