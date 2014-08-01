//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_CONTENT_PNGTEXTUREREADER_C00C0E02_8156_45AD_80DA_DD3B4D89302B_HPP
#define POMDOG_SRC_CONTENT_PNGTEXTUREREADER_C00C0E02_8156_45AD_80DA_DD3B4D89302B_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Graphics/detail/ForwardDeclarations.hpp>
#include <cstdint>
#include <cstddef>
#include <memory>

namespace Pomdog {
namespace Details {

namespace PNGTextureReader {

std::shared_ptr<Texture2D> Read(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
	std::uint8_t const* data, std::size_t byteLength);

}// namespace PNGTextureReader

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_CONTENT_PNGTEXTUREREADER_C00C0E02_8156_45AD_80DA_DD3B4D89302B_HPP)
