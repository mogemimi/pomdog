// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/experimental/skeletal2d/blendtrees/animation_graph.hpp"
#include "pomdog/utility/errors.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Spine {

class SkeletonDesc;

[[nodiscard]] std::tuple<std::shared_ptr<Skeletal2D::AnimationGraph>, std::unique_ptr<Error>>
LoadAnimationGraph(const SkeletonDesc& skeletonDesc, const std::string& assetName);

} // namespace Pomdog::Spine
