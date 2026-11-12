// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/experimental/voxelmodel/voxel_model.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string_view>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::vfs {
class FileSystemContext;
} // namespace pomdog::vfs

namespace pomdog::voxelmodel {

/// Loads a `.voxelmodel` file into a VoxelModel that the caller can edit, and
/// checks it with validateVoxelModel().
[[nodiscard]] POMDOG_EXPORT std::tuple<VoxelModel, std::unique_ptr<Error>>
loadVoxelModel(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    std::string_view filePath) noexcept;

} // namespace pomdog::voxelmodel
