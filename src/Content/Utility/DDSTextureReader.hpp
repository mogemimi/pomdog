//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_CONTENT_DDSTEXTUREREADER_0C8072DF_9382_4D3F_9812_DDBF397296C6_HPP
#define POMDOG_SRC_CONTENT_DDSTEXTUREREADER_0C8072DF_9382_4D3F_9812_DDBF397296C6_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <cstdint>
#include <cstddef>
#include <memory>
#include <Pomdog/Graphics/detail/ForwardDeclarations.hpp>

namespace Pomdog {
namespace Details {

namespace DDSTextureReader {

std::shared_ptr<Texture2D> Read(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
	std::uint8_t const* data, std::size_t byteLength);

}// namespace DDSTextureReader

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_CONTENT_DDSTEXTUREREADER_0C8072DF_9382_4D3F_9812_DDBF397296C6_HPP)
