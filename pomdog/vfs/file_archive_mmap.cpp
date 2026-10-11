// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/vfs/file_archive_mmap.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/flatbuffers_macros.h"
#include "pomdog/basic/platform.h"
#include "pomdog/filesystem/filesystem_path.h"
#include "pomdog/memory/unsafe_ptr.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/string_hash64.h"
#include "pomdog/vfs/file.h"
#include "pomdog/vfs/file_archive_compressed.h"
#include "pomdog/vfs/file_archive_entry.h"
#include "pomdog/vfs/mount_volume.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_BEGIN
#include "pomdogschemas/archive_generated.h"
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_END

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cstring>
#include <memory>
#include <span>
#include <string_view>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

#if defined(POMDOG_PLATFORM_WIN32)
#include "pomdog/basic/win32/windows_headers.h"
#else
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END
#endif

namespace pomdog::vfs {
namespace {

#if defined(POMDOG_PLATFORM_WIN32)

/// MmapRegionWindows provides RAII management for a memory-mapped file region
/// on Windows using CreateFileMapping/MapViewOfFile.
class MmapRegionWindows final {
private:
    unsafe_ptr<void> data_ = nullptr;
    std::size_t size_ = 0;
    HANDLE fileHandle_ = INVALID_HANDLE_VALUE;
    HANDLE mappingHandle_ = nullptr;

public:
    MmapRegionWindows() noexcept = default;

    MmapRegionWindows(const MmapRegionWindows&) = delete;
    MmapRegionWindows& operator=(const MmapRegionWindows&) = delete;
    MmapRegionWindows(MmapRegionWindows&&) = delete;
    MmapRegionWindows& operator=(MmapRegionWindows&&) = delete;

    ~MmapRegionWindows() noexcept
    {
        close();
    }

    [[nodiscard]] std::unique_ptr<Error>
    open(const std::string& path) noexcept
    {
        // NOTE: Share delete access, so that the archive tools can rename a new
        // archive over the file while it stays mapped here with the old content.
        fileHandle_ = CreateFileW(
            platformfs::toFilesystemPath(path).c_str(),
            GENERIC_READ,
            FILE_SHARE_READ | FILE_SHARE_DELETE,
            nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            nullptr);

        if (fileHandle_ == INVALID_HANDLE_VALUE) {
            return errors::make("failed to open file for mapping: " + path);
        }

        LARGE_INTEGER fileSize;
        if (GetFileSizeEx(fileHandle_, &fileSize) == 0) {
            close();
            return errors::make("failed to get file size: " + path);
        }

        size_ = static_cast<std::size_t>(fileSize.QuadPart);
        if (size_ == 0) {
            close();
            return errors::make("cannot map empty file: " + path);
        }

        mappingHandle_ = CreateFileMappingW(
            fileHandle_,
            nullptr,
            PAGE_READONLY,
            0, 0,
            nullptr);

        if (mappingHandle_ == nullptr) {
            close();
            return errors::make("failed to create file mapping: " + path);
        }

        data_ = MapViewOfFile(
            mappingHandle_,
            FILE_MAP_READ,
            0, 0,
            0);

        if (data_ == nullptr) {
            close();
            return errors::make("failed to map view of file: " + path);
        }

        return nullptr;
    }

    void close() noexcept
    {
        if (data_ != nullptr) {
            UnmapViewOfFile(data_);
            data_ = nullptr;
        }
        if (mappingHandle_ != nullptr) {
            CloseHandle(mappingHandle_);
            mappingHandle_ = nullptr;
        }
        if (fileHandle_ != INVALID_HANDLE_VALUE) {
            CloseHandle(fileHandle_);
            fileHandle_ = INVALID_HANDLE_VALUE;
        }
        size_ = 0;
    }

    [[nodiscard]] const u8* data() const noexcept
    {
        return static_cast<const u8*>(data_);
    }

    [[nodiscard]] std::size_t size() const noexcept
    {
        return size_;
    }
};

using MmapRegion = MmapRegionWindows;

#else // POSIX (Linux, macOS)

/// MmapRegionPOSIX provides RAII management for a memory-mapped file region
/// on POSIX systems (Linux, macOS) using mmap.
class MmapRegionPOSIX final {
private:
    unsafe_ptr<void> data_ = nullptr;
    std::size_t size_ = 0;
    int fd_ = -1;

public:
    MmapRegionPOSIX() noexcept = default;

    MmapRegionPOSIX(const MmapRegionPOSIX&) = delete;
    MmapRegionPOSIX& operator=(const MmapRegionPOSIX&) = delete;
    MmapRegionPOSIX(MmapRegionPOSIX&&) = delete;
    MmapRegionPOSIX& operator=(MmapRegionPOSIX&&) = delete;

    ~MmapRegionPOSIX() noexcept
    {
        close();
    }

    [[nodiscard]] std::unique_ptr<Error>
    open(const std::string& path) noexcept
    {
        fd_ = ::open(path.c_str(), O_RDONLY);
        if (fd_ < 0) {
            return errors::make("failed to open file for mapping: " + path);
        }

        struct ::stat st = {};
        if (::fstat(fd_, &st) != 0) {
            close();
            return errors::make("failed to stat file: " + path);
        }

        size_ = static_cast<std::size_t>(st.st_size);
        if (size_ == 0) {
            close();
            return errors::make("cannot map empty file: " + path);
        }

        data_ = ::mmap(nullptr, size_, PROT_READ, MAP_PRIVATE, fd_, 0);
        if (data_ == MAP_FAILED) {
            data_ = nullptr;
            close();
            return errors::make("failed to mmap file: " + path);
        }

        return nullptr;
    }

    void close() noexcept
    {
        if (data_ != nullptr) {
            ::munmap(data_, size_);
            data_ = nullptr;
        }
        if (fd_ >= 0) {
            ::close(fd_);
            fd_ = -1;
        }
        size_ = 0;
    }

    [[nodiscard]] const u8* data() const noexcept
    {
        return static_cast<const u8*>(data_);
    }

    [[nodiscard]] std::size_t size() const noexcept
    {
        return size_;
    }
};

using MmapRegion = MmapRegionPOSIX;

#endif

/// ArchiveFileMmap represents a single file entry within a memory-mapped archive.
/// It reads data from a region of the mapped .pak file via memcpy.
class ArchiveFileMmap final : public File {
private:
    std::shared_ptr<MmapRegion> pakRegion_;
    std::size_t offset_ = 0;
    std::size_t size_ = 0;
    std::size_t position_ = 0;

public:
    ArchiveFileMmap() noexcept = default;

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        std::shared_ptr<MmapRegion> pakRegion,
        std::size_t offset,
        std::size_t size) noexcept
    {
        pakRegion_ = std::move(pakRegion);
        if (!pakRegion_ || pakRegion_->data() == nullptr) {
            return errors::make("invalid pak region for archive file");
        }
        if (offset > pakRegion_->size()) {
            return errors::make("file offset exceeds pak region size");
        }
        if (size > pakRegion_->size() - offset) {
            return errors::make("file range exceeds pak region size");
        }

        offset_ = offset;
        size_ = size;
        position_ = 0;

        return nullptr;
    }

    [[nodiscard]] std::tuple<std::size_t, std::unique_ptr<Error>>
    read(std::span<u8> buffer) noexcept override
    {
        if (pakRegion_ == nullptr) {
            return {0, errors::make("file is closed")};
        }

        if (size_ < position_) {
            return {0, errors::make("invalid file position")};
        }

        const auto remaining = size_ - position_;
        if (remaining == 0) {
            return {0, nullptr};
        }

        const auto toRead = std::min(buffer.size(), remaining);
        std::memcpy(buffer.data(), pakRegion_->data() + offset_ + position_, toRead);
        position_ += toRead;

        return {toRead, nullptr};
    }

    [[nodiscard]] std::tuple<std::size_t, std::unique_ptr<Error>>
    write([[maybe_unused]] std::span<const u8> buffer) noexcept override
    {
        return {0, errors::make("archive files are read-only")};
    }

    [[nodiscard]] std::tuple<i64, std::unique_ptr<Error>>
    seek(i64 offset, SeekOrigin origin) noexcept override
    {
        i64 newPos = 0;
        switch (origin) {
        case SeekOrigin::Begin:
            newPos = offset;
            break;
        case SeekOrigin::Current:
            newPos = static_cast<i64>(position_) + offset;
            break;
        case SeekOrigin::End:
            newPos = static_cast<i64>(size_) + offset;
            break;
        }

        if (newPos < 0 || static_cast<std::size_t>(newPos) > size_) {
            return {0, errors::make("seek out of range")};
        }

        position_ = static_cast<std::size_t>(newPos);
        return {newPos, nullptr};
    }

    [[nodiscard]] std::unique_ptr<Error>
    close() noexcept override
    {
        pakRegion_ = nullptr;
        return nullptr;
    }

    [[nodiscard]] std::tuple<FileInfo, std::unique_ptr<Error>>
    stat() noexcept override
    {
        FileInfo info = {};
        info.size = size_;
        info.isDirectory = false;
        return {std::move(info), nullptr};
    }

    [[nodiscard]] std::tuple<std::size_t, std::unique_ptr<Error>>
    getSize() noexcept override
    {
        return {size_, nullptr};
    }
};

/// ArchiveEntryReaderMmap reads the stored bytes of an archive entry from a
/// memory-mapped .pak file without copying them.
class ArchiveEntryReaderMmap final : public detail::ArchiveEntryReader {
private:
    std::shared_ptr<MmapRegion> pakRegion_;
    std::span<const u8> bytes_;

public:
    ArchiveEntryReaderMmap() noexcept = default;

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        std::shared_ptr<MmapRegion> pakRegion,
        std::size_t offset,
        std::size_t size) noexcept
    {
        pakRegion_ = std::move(pakRegion);
        if (!pakRegion_ || pakRegion_->data() == nullptr) {
            return errors::make("invalid pak region for archive entry");
        }
        if (offset > pakRegion_->size() || size > pakRegion_->size() - offset) {
            return errors::make("archive entry range exceeds pak region size");
        }

        POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
        bytes_ = std::span<const u8>{pakRegion_->data() + offset, size};
        POMDOG_CLANG_UNSAFE_BUFFER_END

        return nullptr;
    }

    [[nodiscard]] std::tuple<std::span<const u8>, std::unique_ptr<Error>>
    read(std::size_t offset) noexcept override
    {
        if (offset >= bytes_.size()) {
            return {std::span<const u8>{}, nullptr};
        }

        // NOTE: The whole entry is mapped, so one span holds the rest of it.
        return {bytes_.subspan(offset), nullptr};
    }
};

/// ArchiveVolumeMmap implements the MountVolume interface for a memory-mapped archive.
/// Both the .idx (FlatBuffers index) and .pak (stored entry data) files are memory-mapped.
class ArchiveVolumeMmap final : public MountVolume {
private:
    MmapRegion idxRegion_;
    std::shared_ptr<MmapRegion> pakRegion_;
    std::vector<std::shared_ptr<const detail::ArchiveDictionary>> dictionaries_;

public:
    ArchiveVolumeMmap() noexcept = default;

    ArchiveVolumeMmap(const ArchiveVolumeMmap&) = delete;
    ArchiveVolumeMmap& operator=(const ArchiveVolumeMmap&) = delete;

    [[nodiscard]] std::unique_ptr<Error>
    initialize(const std::string& idxPath, const std::string& pakPath) noexcept
    {
        // NOTE: Memory-map the .idx file
        if (auto err = idxRegion_.open(idxPath); err != nullptr) {
            return errors::wrap(std::move(err), "failed to mmap index file: " + idxPath);
        }

        // NOTE: Verify the FlatBuffer on the mapped memory
        auto verifier = flatbuffers::Verifier(idxRegion_.data(), idxRegion_.size());
        if (!pomdogschemas::VerifyArchiveBuffer(verifier)) {
            return errors::make("invalid archive index: " + idxPath);
        }

        {
            const auto* archive = pomdogschemas::GetArchive(idxRegion_.data());
            if (archive == nullptr) {
                return errors::make("failed to parse archive index: " + idxPath);
            }

            const auto* keys = archive->keys();
            const auto* entries = archive->entries();

            if (keys == nullptr || entries == nullptr) {
                return errors::make("archive index has no keys or entries: " + idxPath);
            }

            // NOTE: Create the decoding dictionaries once, so that they stay in
            // memory for every entry compressed with them.
            auto [dictionaries, dictionariesErr] = detail::createArchiveDictionaries(*archive);
            if (dictionariesErr != nullptr) {
                return errors::wrap(std::move(dictionariesErr), "invalid archive index: " + idxPath);
            }
            dictionaries_ = std::move(dictionaries);
        }

        // NOTE: Memory-map the .pak file
        pakRegion_ = std::make_shared<MmapRegion>();
        if (auto err = pakRegion_->open(pakPath); err != nullptr) {
            return errors::wrap(std::move(err), "failed to mmap pak file: " + pakPath);
        }

        return nullptr;
    }

    [[nodiscard]] std::tuple<std::shared_ptr<File>, std::unique_ptr<Error>>
    open(std::string_view path, const FileMode& mode) noexcept override
    {
        return openByHash(computeStringHash64(path), mode);
    }

    [[nodiscard]] std::tuple<std::shared_ptr<File>, std::unique_ptr<Error>>
    open(u64 hashKey, const FileMode& mode) noexcept override
    {
        return openByHash(hashKey, mode);
    }

    [[nodiscard]] bool
    exists(std::string_view path) noexcept override
    {
        return existsByHash(computeStringHash64(path));
    }

    [[nodiscard]] bool
    exists(u64 hashKey) noexcept override
    {
        return existsByHash(hashKey);
    }

    [[nodiscard]] std::tuple<FileInfo, std::unique_ptr<Error>>
    stat(std::string_view path) noexcept override
    {
        return statByHash(computeStringHash64(path));
    }

    [[nodiscard]] std::tuple<FileInfo, std::unique_ptr<Error>>
    stat(u64 hashKey) noexcept override
    {
        return statByHash(hashKey);
    }

    [[nodiscard]] std::tuple<std::string, std::unique_ptr<Error>>
    toPhysicalPath([[maybe_unused]] std::string_view path) noexcept override
    {
        return {"", errors::make("archive volumes do not have physical paths")};
    }

private:
    [[nodiscard]] std::tuple<std::shared_ptr<File>, std::unique_ptr<Error>>
    openByHash(u64 key, const FileMode& mode) noexcept
    {
        if (mode.write || mode.create || mode.append || mode.truncate) {
            return {nullptr, errors::make("archive files are read-only")};
        }

        const auto* archive = pomdogschemas::GetArchive(idxRegion_.data());
        auto [entry, entryErr] = detail::findArchiveEntry(*archive, key, pakRegion_->size());
        if (entryErr != nullptr) {
            return {nullptr, std::move(entryErr)};
        }

        const auto offset = static_cast<std::size_t>(entry->start_offset());
        const auto storedSize = static_cast<std::size_t>(entry->compressed_size());

        if (entry->compression() == pomdogschemas::ArchiveCompression_None) {
            auto file = std::make_shared<ArchiveFileMmap>();
            if (auto err = file->initialize(pakRegion_, offset, storedSize); err != nullptr) {
                return {nullptr, errors::wrap(std::move(err), "failed to initialize archive file")};
            }
            return {std::move(file), nullptr};
        }

        auto reader = std::make_unique<ArchiveEntryReaderMmap>();
        if (auto err = reader->initialize(pakRegion_, offset, storedSize); err != nullptr) {
            return {nullptr, errors::wrap(std::move(err), "failed to initialize archive entry reader")};
        }

        // NOTE: findArchiveEntry() checks that the archive has the dictionary.
        std::shared_ptr<const detail::ArchiveDictionary> dictionary;
        if (entry->compression() == pomdogschemas::ArchiveCompression_ZstdDictionary) {
            dictionary = dictionaries_[entry->dictionary_index()];
        }

        auto file = std::make_shared<detail::CompressedArchiveFile>(
            std::move(reader),
            entry->compression(),
            std::move(dictionary),
            static_cast<std::size_t>(entry->uncompressed_size()));
        return {std::move(file), nullptr};
    }

    [[nodiscard]] bool
    existsByHash(u64 key) noexcept
    {
        const auto* archive = pomdogschemas::GetArchive(idxRegion_.data());
        const auto* keys = archive->keys();
        POMDOG_ASSERT(keys != nullptr);

        return keys->LookupByKey(key) != nullptr;
    }

    [[nodiscard]] std::tuple<FileInfo, std::unique_ptr<Error>>
    statByHash(u64 key) noexcept
    {
        const auto* archive = pomdogschemas::GetArchive(idxRegion_.data());
        auto [entry, entryErr] = detail::findArchiveEntry(*archive, key, pakRegion_->size());
        if (entryErr != nullptr) {
            return {FileInfo{}, std::move(entryErr)};
        }

        FileInfo info = {};
        info.size = static_cast<std::size_t>(entry->uncompressed_size());
        info.isDirectory = false;

        return {std::move(info), nullptr};
    }
};

} // namespace

std::tuple<std::shared_ptr<MountVolume>, std::unique_ptr<Error>>
openArchiveFileMmap(const std::string& idxPath, const std::string& pakPath) noexcept
{
    auto volume = std::make_shared<ArchiveVolumeMmap>();
    if (auto err = volume->initialize(idxPath, pakPath); err != nullptr) {
        return {nullptr, errors::wrap(std::move(err), "volume->initialize() failed")};
    }
    return {std::move(volume), nullptr};
}

} // namespace pomdog::vfs
