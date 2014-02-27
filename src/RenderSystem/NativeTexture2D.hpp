//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_RENDERSYSTEM_NATIVETEXTURE2D_309E0647_8B22_4554_A9E8_8269BC1035C5_HPP
#define POMDOG_SRC_RENDERSYSTEM_NATIVETEXTURE2D_309E0647_8B22_4554_A9E8_8269BC1035C5_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Config/FundamentalTypes.hpp>

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeTexture2D
{
public:
	NativeTexture2D() = default;
	NativeTexture2D(NativeTexture2D const&) = delete;
	NativeTexture2D & operator=(NativeTexture2D const&) = delete;

	virtual ~NativeTexture2D() = default;
	
	virtual void SetData(std::uint32_t pixelWidth, std::uint32_t pixelHeight,
		std::uint32_t levelCount, SurfaceFormat format, void const* pixelData) = 0;
	
	virtual void Apply(std::uint32_t index) = 0;
	
	///@~Japanese
	/// @brief バインドされているテクスチャをテクスチャユニットから外します。
	virtual void UnbindFromTextureUnit(std::uint32_t index) = 0;
};

}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_RENDERSYSTEM_NATIVETEXTURE2D_309E0647_8B22_4554_A9E8_8269BC1035C5_HPP)
