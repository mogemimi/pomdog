// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string_view>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::vfs {
class File;
class FileSystemContext;
} // namespace pomdog::vfs

namespace pomdogschemas {
struct Skeletal2DSkeleton;
} // namespace pomdogschemas

namespace pomdog::spine {

/// SkeletonData is the joint hierarchy of a `.skeleton_skel2d` file, which
/// `spine-json-to-skeletal2d` converts offline from a Spine JSON file.
///
/// It keeps the file open and reads the FlatBuffers table from its view, so
/// that a memory-mapped archive serves the joints without copying them.
class SkeletonData final {
private:
    std::shared_ptr<vfs::File> file_;
    const pomdogschemas::Skeletal2DSkeleton* table_ = nullptr;

public:
    /// Takes a table in the view of `file` that `loadSkeletonData()` has verified.
    SkeletonData(
        std::shared_ptr<vfs::File> file,
        const pomdogschemas::Skeletal2DSkeleton* table) noexcept;

    /// Returns the verified table. It has between 1 and 255 joints, the first
    /// joint is the root, and the parent of every other joint precedes it.
    [[nodiscard]] const pomdogschemas::Skeletal2DSkeleton&
    getTable() const noexcept;
};

/// Loads a `.skeleton_skel2d` file and verifies its content.
[[nodiscard]] std::tuple<std::shared_ptr<SkeletonData>, std::unique_ptr<Error>>
loadSkeletonData(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    std::string_view filePath) noexcept;

} // namespace pomdog::spine
