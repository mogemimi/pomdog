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

/// Opens an archive (.idx + .pak) as a MountVolume.
///
/// The .idx file is a FlatBuffers-serialized index that maps xxHash-64 keys
/// to offsets and sizes in the .pak file. The .pak file contains the
/// concatenated raw file data.
/// `idxPath` is the path to the .idx file, and `pakPath` is the path to the .pak file.
/// Returns a MountVolume that can be mounted with vfs::mount(), or an error if the
/// archive could not be opened.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<MountVolume>, std::unique_ptr<Error>>
openArchiveFile(const std::string& idxPath, const std::string& pakPath) noexcept;

} // namespace pomdog::vfs
