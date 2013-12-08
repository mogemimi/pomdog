//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Graphics/BlendDescription.hpp>
#include <limits>

namespace Pomdog {

BlendDescription::BlendDescription()
	: BlendFactor(Color::White)
	, ColorBlendFunction	(BlendFunction::Add)
	, AlphaBlendFunction	(BlendFunction::Add)
	, ColorSourceBlend		(Blend::One)
	, AlphaSourceBlend		(Blend::One)
	, ColorDestinationBlend	(Blend::One)
	, AlphaDestinationBlend	(Blend::One)
	, MultiSampleMask(std::numeric_limits<std::uint32_t>::max())
	, AlphaToCoverageEnable(false)
{}

}// namespace Pomdog
