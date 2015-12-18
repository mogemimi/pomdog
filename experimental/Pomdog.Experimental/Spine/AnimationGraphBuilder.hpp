// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_ANIMATIONGRAPHBUILDER_F4A6BA28_HPP
#define POMDOG_ANIMATIONGRAPHBUILDER_F4A6BA28_HPP

#include "Pomdog.Experimental/Skeletal2D/AnimationGraph.hpp"
#include "Pomdog/Content/AssetManager.hpp"
#include <memory>
#include <string>

namespace Pomdog {
namespace Spine {

class SkeletonDesc;

std::shared_ptr<AnimationGraph> LoadAnimationGraph(SkeletonDesc const& skeletonDesc,
    AssetManager const& assets, std::string const& assetName);

} // namespace Spine
} // namespace Pomdog

#endif // POMDOG_ANIMATIONGRAPHBUILDER_F4A6BA28_HPP
