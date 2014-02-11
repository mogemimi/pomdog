//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "Texture2DGL4.hpp"
#include <Pomdog/Utility/Exception.hpp>

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace GL4 {

Texture2DGL4::Texture2DGL4(std::uint32_t pixelWidth, std::uint32_t pixelHeight,
	std::uint32_t mipmapLevels, SurfaceFormat surfaceFormat)
{
	///@todo Not implemented
	POMDOG_THROW_EXCEPTION(std::runtime_error,
		"Not implemented", "Texture2DGL4::Texture2DGL4");
}

}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog
