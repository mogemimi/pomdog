// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/vfs/mount_volume.h"
#include "pomdog/utility/errors.h"
#include "pomdog/vfs/file.h"

namespace pomdog::vfs {

MountVolume::MountVolume() noexcept = default;

MountVolume::~MountVolume() noexcept = default;

std::tuple<std::shared_ptr<File>, std::unique_ptr<Error>>
MountVolume::open([[maybe_unused]] u64 hashKey, [[maybe_unused]] const FileMode& mode) noexcept
{
    return {nullptr, errors::make("hash-key lookup is not supported by this volume")};
}

bool MountVolume::exists([[maybe_unused]] u64 hashKey) noexcept
{
    return false;
}

std::tuple<FileInfo, std::unique_ptr<Error>>
MountVolume::stat([[maybe_unused]] u64 hashKey) noexcept
{
    return {FileInfo{}, errors::make("hash-key lookup is not supported by this volume")};
}

} // namespace pomdog::vfs
