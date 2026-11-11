// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/skeletal2d/blendtrees/animation_graph.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string_view>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::vfs {
class FileSystemContext;
} // namespace pomdog::vfs

namespace pomdog::skeletal2d {

class AnimationClipData;

/// Loads an animation graph from a `.animgraph_skel2d` file, which
/// `animation-graph-json-to-skeletal2d` converts offline from a JSON file.
/// The states and parameters of the graph have the xxHash32 hashes of their
/// names as keys, which `skeletal2d::Animator` takes.
///
/// The clip nodes of the graph play the clips of `clipData`, which must have
/// every clip that the graph refers to.
[[nodiscard]] std::tuple<std::shared_ptr<AnimationGraph>, std::unique_ptr<Error>>
loadAnimationGraph(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const AnimationClipData& clipData,
    std::string_view filePath);

} // namespace pomdog::skeletal2d
