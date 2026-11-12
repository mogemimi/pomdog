// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::vfs {
class FileSystemContext;
} // namespace pomdog::vfs

namespace pomdog::voxelmodel {
struct VoxelModel;
} // namespace pomdog::voxelmodel

namespace pomdog::voxelmodel {

/// Writes a VoxelModel to a `.voxelmodel` file, replacing the file when it
/// exists. The file must be on a writable volume of `fs`. Returns an error
/// without writing when validateVoxelModel() rejects the model.
[[nodiscard]] POMDOG_EXPORT std::unique_ptr<Error>
saveVoxelModel(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    std::string_view filePath,
    const VoxelModel& model) noexcept;

} // namespace pomdog::voxelmodel
