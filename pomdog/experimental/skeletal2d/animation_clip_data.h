// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"

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
struct Skeletal2DClip;
struct Skeletal2DClipSet;
} // namespace pomdogschemas

namespace pomdog::skeletal2d {

/// AnimationClipData is the animation clips of a `.clip_skel2d` file, which
/// `spine-json-to-skeletal2d` converts offline from a Spine JSON file.
///
/// It keeps the file open and reads the FlatBuffers tables from its view, so
/// that a memory-mapped archive serves the clips without copying them.
///
/// The clips refer to joints by index, so use them with the skeleton file
/// that was converted from the same source.
class AnimationClipData final {
private:
    std::shared_ptr<vfs::File> file_;
    const pomdogschemas::Skeletal2DClipSet* table_ = nullptr;

public:
    /// Takes a table in the view of `file` that `loadAnimationClipData()` has verified.
    AnimationClipData(
        std::shared_ptr<vfs::File> file,
        const pomdogschemas::Skeletal2DClipSet* table) noexcept;

    /// Returns the clip whose name has the given xxHash32 key, or nullptr when
    /// the file has no such clip.
    ///
    /// The clip is verified. Every track animates a joint or a slot and has
    /// keys, which are in ascending order of time and within the value ranges
    /// that `schemas/skeletal2d_clip.fbs` sets.
    [[nodiscard]] const pomdogschemas::Skeletal2DClip*
    findClip(u32 key) const noexcept;
};

/// Loads a `.clip_skel2d` file and verifies its content.
[[nodiscard]] std::tuple<std::shared_ptr<AnimationClipData>, std::unique_ptr<Error>>
loadAnimationClipData(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    std::string_view filePath) noexcept;

} // namespace pomdog::skeletal2d
