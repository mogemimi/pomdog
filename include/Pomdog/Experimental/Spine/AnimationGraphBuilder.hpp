// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/Skeletal2D/BlendTrees/AnimationGraph.hpp"
#include "Pomdog/Utility/Errors.hpp"
#include <memory>
#include <string>
#include <tuple>

namespace Pomdog::Spine {

class SkeletonDesc;

[[nodiscard]] std::tuple<std::shared_ptr<Skeletal2D::AnimationGraph>, std::shared_ptr<Error>>
LoadAnimationGraph(const SkeletonDesc& skeletonDesc, const std::string& assetName);

} // namespace Pomdog::Spine
