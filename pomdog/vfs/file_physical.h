// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::vfs {
class MountVolume;
} // namespace pomdog::vfs

namespace pomdog::vfs {

/// Creates a MountVolume backed by a physical directory on disk.
///
/// `rootDir` is the path to the root directory of the volume on the OS filesystem.
/// Returns a MountVolume that can be mounted with vfs::mount(), or an error if the
/// directory could not be opened.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<MountVolume>, std::unique_ptr<Error>>
createMountVolumePhysical(std::string rootDir) noexcept;

} // namespace pomdog::vfs
