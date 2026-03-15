// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/skeletal2d/blendtrees/animation_graph.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::vfs {
class FileSystemContext;
} // namespace pomdog::vfs

namespace pomdog::spine {
class SkeletonDesc;
} // namespace pomdog::spine

namespace pomdog::spine {

[[nodiscard]] std::tuple<std::shared_ptr<skeletal2d::AnimationGraph>, std::unique_ptr<Error>>
loadAnimationGraph(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const SkeletonDesc& skeletonDesc,
    const std::string& assetName);

} // namespace pomdog::spine
