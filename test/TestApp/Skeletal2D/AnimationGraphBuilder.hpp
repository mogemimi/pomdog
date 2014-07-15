//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_ANIMATIONGRAPHBUILDER_F4A6BA28_8259_4FC0_AED3_95C0FA371A26_HPP
#define POMDOG_ANIMATIONGRAPHBUILDER_F4A6BA28_8259_4FC0_AED3_95C0FA371A26_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <memory>
#include <string>
#include <Pomdog/Pomdog.hpp>
#include "AnimationGraph.hpp"
#include "../Spine/SkeletonDesc.hpp"

namespace Pomdog {
namespace Details {
namespace Skeletal2D {

std::shared_ptr<AnimationGraph> LoadAnimationGraph(Details::Skeletal2D::SkeletonDesc const& skeletonDesc,
	AssetManager const& assets, std::string const& assetPath);

}// namespace Skeletal2D
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_ANIMATIONGRAPHBUILDER_F4A6BA28_8259_4FC0_AED3_95C0FA371A26_HPP)
