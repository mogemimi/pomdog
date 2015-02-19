//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_PRESENTATIONPARAMETERS_2A860062_8271_4B7E_A8D7_1279CEF49E8F_HPP
#define POMDOG_PRESENTATIONPARAMETERS_2A860062_8271_4B7E_A8D7_1279CEF49E8F_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "Pomdog/Graphics/DepthFormat.hpp"
#include "Pomdog/Graphics/SurfaceFormat.hpp"
#include <cstdint>

namespace Pomdog {

class PresentationParameters {
public:
	std::uint32_t BackBufferWidth = 800;
	std::uint32_t BackBufferHeight = 480;
	std::uint32_t PresentationInterval = 60;
	SurfaceFormat SurfaceFormat = SurfaceFormat::R8G8B8A8_UNorm;
	DepthFormat DepthFormat = DepthFormat::Depth24Stencil8;
	bool IsFullScreen = false;
};

}// namespace Pomdog

#endif // !defined(POMDOG_PRESENTATIONPARAMETERS_2A860062_8271_4B7E_A8D7_1279CEF49E8F_HPP)
