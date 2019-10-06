// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Skeletal2D/AnimationGraph.hpp"
#include "Pomdog/Utility/Errors.hpp"
#include <memory>
#include <string>

namespace Pomdog::Spine {

class SkeletonDesc;

[[nodiscard]] std::tuple<std::shared_ptr<AnimationGraph>, std::shared_ptr<Error>>
LoadAnimationGraph(const SkeletonDesc& skeletonDesc, const std::string& assetName);

} // namespace Pomdog::Spine
