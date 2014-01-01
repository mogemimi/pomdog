//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Graphics/SamplerDescription.hpp>
#include <limits>

namespace Pomdog {

SamplerDescription::SamplerDescription()
	: MaxAnisotropy(0)
	, MinMipLevel(std::numeric_limits<float>::min())
	, MaxMipLevel(std::numeric_limits<float>::max())
	, Filter(TextureFilter::Linear)
	, AddressU(TextureAddressMode::Clamp)
	, AddressV(TextureAddressMode::Clamp)
	, AddressW(TextureAddressMode::Clamp)
	//, BorderColor(Color::TransparentBlack)
	//, MipMapLevelOfDetailBias(0.0f)
{}

}// namespace Pomdog
