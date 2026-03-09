// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/vfs/file_archive.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/flatbuffers_macros.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/string_hash64.h"
#include "pomdog/vfs/file.h"
#include "pomdog/vfs/mount_volume.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_BEGIN
#include "pomdogschemas/archive_generated.h"
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_END

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::vfs {
namespace {

/// ArchiveFile represents a single file entry within the archive. It reads data
/// from a specific range within a .pak file. It does not support writing or seeking,
/// and is read-only.
class ArchiveFile final : public File {
private:
    std::shared_ptr<std::ifstream> stream_;
    std::size_t offset_ = 0;
    std::size_t size_ = 0;
    std::size_t position_ = 0;

public:
    ArchiveFile() noexcept = default;

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        std::shared_ptr<std::ifstream> stream,
        std::size_t offset,
        std::size_t size) noexcept
    {
        stream_ = std::move(stream);
        if (!stream_) {
            return errors::make("invalid stream for archive file");
        }

        offset_ = offset;
        size_ = size;
        position_ = 0;

        return nullptr;
    }

    [[nodiscard]] std::tuple<std::size_t, std::unique_ptr<Error>>
    read(std::span<u8> buffer) noexcept override
    {
        if (stream_ == nullptr) {
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

        stream_->seekg(
            static_cast<std::streamoff>(offset_ + position_),
            std::ios::beg);

        if (!stream_->good()) {
            return {0, errors::make("seek failed in archive")};
        }

        stream_->read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(toRead));
        const auto bytesRead = static_cast<std::size_t>(stream_->gcount());
        position_ += bytesRead;

        if (stream_->bad()) {
            return {bytesRead, errors::make("read failed in archive")};
        }

        return {bytesRead, nullptr};
    }

    [[nodiscard]] std::tuple<std::size_t, std::unique_ptr<Error>>
    write([[maybe_unused]] std::span<const u8> buffer) noexcept override
    {
        return {0, errors::make("archive files are read-only")};
    }

    [[nodiscard]] std::unique_ptr<Error>
    close() noexcept override
    {
        stream_ = nullptr;
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

/// ArchiveVolume implements the MountVolume interface for an archive consisting of
/// a .idx file (a FlatBuffers-serialized index) and a .pak file (a concatenated raw file data).
class ArchiveVolume final : public MountVolume {
private:
    std::unique_ptr<u8[]> storage_;
    std::shared_ptr<std::ifstream> pakStream_;

public:
    ArchiveVolume() noexcept = default;

    ArchiveVolume(const ArchiveVolume&) = delete;
    ArchiveVolume& operator=(const ArchiveVolume&) = delete;

    [[nodiscard]] std::unique_ptr<Error>
    initialize(const std::string& idxPath, const std::string& pakPath) noexcept
    {
        // NOTE: Read the .idx file into memory
        std::ifstream idxFile(idxPath, std::ios::binary);
        if (!idxFile.is_open()) {
            return errors::make("failed to open index file: " + idxPath);
        }

        std::error_code err = {};
        const auto idxSize = static_cast<std::size_t>(std::filesystem::file_size(idxPath, err));
        if (err) {
            return errors::make("failed to get index file size: " + idxPath);
        }
        if (idxSize == 0) {
            return errors::make("empty index file: " + idxPath);
        }
        if (idxSize > static_cast<std::size_t>(std::numeric_limits<std::streamsize>::max())) {
            return errors::make("index file is too large: " + idxPath);
        }

        storage_ = std::make_unique<u8[]>(idxSize);

        idxFile.seekg(0, std::ios::beg);
        idxFile.read(reinterpret_cast<char*>(storage_.get()), static_cast<std::streamsize>(idxSize));
        if (!idxFile.good()) {
            return errors::make("failed to read index file: " + idxPath);
        }
        idxFile.close();

        // NOTE: Verify the FlatBuffer
        auto verifier = flatbuffers::Verifier(storage_.get(), idxSize);
        if (!pomdogschemas::VerifyArchiveBuffer(verifier)) {
            return errors::make("invalid archive index: " + idxPath);
        }

        {
            const auto* archive = pomdogschemas::GetArchive(storage_.get());
            if (archive == nullptr) {
                return errors::make("failed to parse archive index: " + idxPath);
            }

            // NOTE: Parse entries
            const auto* keys = archive->keys();
            const auto* entries = archive->entries();

            if (keys == nullptr || entries == nullptr) {
                return errors::make("archive index has no keys or entries: " + idxPath);
            }
        }

        // NOTE: Open the .pak file
        pakStream_ = std::make_shared<std::ifstream>(pakPath, std::ios::binary);
        if (!pakStream_->is_open()) {
            return errors::make("failed to open pak file: " + pakPath);
        }

        return nullptr;
    }

    [[nodiscard]] std::tuple<std::shared_ptr<File>, std::unique_ptr<Error>>
    open(const std::string& path, const FileMode& mode) noexcept override
    {
        return openByHash(computeStringHash64(path), mode);
    }

    [[nodiscard]] std::tuple<std::shared_ptr<File>, std::unique_ptr<Error>>
    open(u64 hashKey, const FileMode& mode) noexcept override
    {
        return openByHash(hashKey, mode);
    }

    [[nodiscard]] bool
    exists(const std::string& path) noexcept override
    {
        return existsByHash(computeStringHash64(path));
    }

    [[nodiscard]] bool
    exists(u64 hashKey) noexcept override
    {
        return existsByHash(hashKey);
    }

    [[nodiscard]] std::tuple<FileInfo, std::unique_ptr<Error>>
    stat(const std::string& path) noexcept override
    {
        return statByHash(computeStringHash64(path));
    }

    [[nodiscard]] std::tuple<FileInfo, std::unique_ptr<Error>>
    stat(u64 hashKey) noexcept override
    {
        return statByHash(hashKey);
    }

    [[nodiscard]] std::tuple<std::string, std::unique_ptr<Error>>
    toPhysicalPath([[maybe_unused]] const std::string& path) noexcept override
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

        const auto archive = pomdogschemas::GetArchive(storage_.get());
        const auto keys = archive->keys();

        POMDOG_ASSERT(keys != nullptr);

        const auto result = keys->LookupByKey(key);
        if (result == nullptr) {
            return {nullptr, errors::make("file not found in archive")};
        }

        const auto entries = archive->entries();
        POMDOG_ASSERT(entries != nullptr);

        const auto index = result->index();
        static_assert(std::is_unsigned_v<decltype(index)>, "index must be >= 0");
        if (index >= entries->size()) {
            return {nullptr, errors::make("invalid archive index")};
        }

        const auto entry = entries->Get(index);
        if (entry == nullptr) {
            return {nullptr, errors::make("file not found in archive")};
        }

        if (entry->compressed()) {
            return {nullptr, errors::make("compressed archive entries are not yet supported")};
        }

        auto file = std::make_shared<ArchiveFile>();

        if (auto err = file->initialize(
                pakStream_,
                static_cast<std::size_t>(entry->start_offset()),
                static_cast<std::size_t>(entry->uncompressed_size()));
            err != nullptr) {
            return {nullptr, errors::wrap(std::move(err), "failed to initialize archive file")};
        }

        return {std::move(file), nullptr};
    }

    [[nodiscard]] bool
    existsByHash(u64 key) noexcept
    {
        const auto archive = pomdogschemas::GetArchive(storage_.get());
        const auto keys = archive->keys();

        POMDOG_ASSERT(keys != nullptr);

        return keys->LookupByKey(key) != nullptr;
    }

    [[nodiscard]] std::tuple<FileInfo, std::unique_ptr<Error>>
    statByHash(u64 key) noexcept
    {
        const auto archive = pomdogschemas::GetArchive(storage_.get());
        const auto keys = archive->keys();

        POMDOG_ASSERT(keys != nullptr);

        const auto result = keys->LookupByKey(key);
        if (result == nullptr) {
            return {FileInfo{}, errors::make("file not found in archive")};
        }

        const auto entries = archive->entries();
        POMDOG_ASSERT(entries != nullptr);

        const auto index = result->index();
        static_assert(std::is_unsigned_v<decltype(index)>, "index must be >= 0");
        if (index >= entries->size()) {
            return {FileInfo{}, errors::make("invalid archive index")};
        }

        const auto entry = entries->Get(index);

        FileInfo info = {};
        info.size = static_cast<std::size_t>(entry->uncompressed_size());
        info.isDirectory = false;

        return {std::move(info), nullptr};
    }
};

} // namespace

std::tuple<std::shared_ptr<MountVolume>, std::unique_ptr<Error>>
openArchiveFile(const std::string& idxPath, const std::string& pakPath) noexcept
{
    auto volume = std::make_shared<ArchiveVolume>();
    if (auto err = volume->initialize(idxPath, pakPath); err != nullptr) {
        return {nullptr, errors::wrap(std::move(err), "volume->initialize() failed")};
    }
    return {std::move(volume), nullptr};
}

} // namespace pomdog::vfs
