//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SPINE_ANIMATIONGRAPHBUILDER_F4A6BA28_8259_4FC0_AED3_95C0FA371A26_HPP
#define POMDOG_SPINE_ANIMATIONGRAPHBUILDER_F4A6BA28_8259_4FC0_AED3_95C0FA371A26_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "AnimationGraph.hpp"
#include <Pomdog/Pomdog.hpp>
#include <memory>
#include <string>

namespace Pomdog {
namespace Spine {

class SkeletonDesc;

std::shared_ptr<AnimationGraph> LoadAnimationGraph(SkeletonDesc const& skeletonDesc,
	AssetManager const& assets, std::string const& assetName);

}// namespace Spine
}// namespace Pomdog

#endif // !defined(POMDOG_SPINE_ANIMATIONGRAPHBUILDER_F4A6BA28_8259_4FC0_AED3_95C0FA371A26_HPP)
