//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_RENDERSYSTEM_PRESENTATIONPARAMETERS_2A860062_8271_4B7E_A8D7_1279CEF49E8F_HPP
#define POMDOG_SRC_RENDERSYSTEM_PRESENTATIONPARAMETERS_2A860062_8271_4B7E_A8D7_1279CEF49E8F_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include <Pomdog/Graphics/DepthFormat.hpp>
#include <cstdint>

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class PresentationParameters {
public:
	DepthFormat DepthFormat = DepthFormat::Depth24Stencil8;
	std::uint32_t BackBufferWidth = 800;
	std::uint32_t BackBufferHeight = 480;
	bool IsFullScreen = false;
};

}// namespace RenderSystem
}// namespace Detauks
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_RENDERSYSTEM_PRESENTATIONPARAMETERS_2A860062_8271_4B7E_A8D7_1279CEF49E8F_HPP)
