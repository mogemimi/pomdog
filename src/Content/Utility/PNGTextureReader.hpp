//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_PNGTEXTUREREADER_C00C0E02_8156_45AD_80DA_DD3B4D89302B_HPP
#define POMDOG_PNGTEXTUREREADER_C00C0E02_8156_45AD_80DA_DD3B4D89302B_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <cstdint>
#include <cstddef>
#include <memory>

namespace Pomdog {
namespace Details {

struct PNGTextureReader {
	static std::shared_ptr<Texture2D> Read(
		std::shared_ptr<GraphicsDevice> const& graphicsDevice,
		std::uint8_t const* data, std::size_t byteLength);
};

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_PNGTEXTUREREADER_C00C0E02_8156_45AD_80DA_DD3B4D89302B_HPP)
