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

namespace pomdog::magicavoxel {
class VoxModel;
} // namespace pomdog::magicavoxel

namespace pomdog::vfs {
class FileSystemContext;
} // namespace pomdog::vfs

namespace pomdog {

/// Loads a VoxModel from a .vox file via VFS.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<magicavoxel::VoxModel>, std::unique_ptr<Error>>
loadVoxModel(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::string& filePath);

} // namespace pomdog
