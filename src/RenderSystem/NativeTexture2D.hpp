//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_SRC_RENDERSYSTEM_NATIVETEXTURE2D_309E0647_8B22_4554_A9E8_8269BC1035C5_HPP
#define POMDOG_SRC_RENDERSYSTEM_NATIVETEXTURE2D_309E0647_8B22_4554_A9E8_8269BC1035C5_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include <Pomdog/Graphics/detail/ForwardDeclarations.hpp>
#include <cstdint>

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeTexture2D {
public:
	NativeTexture2D() = default;
	NativeTexture2D(NativeTexture2D const&) = delete;
	NativeTexture2D & operator=(NativeTexture2D const&) = delete;

	virtual ~NativeTexture2D() = default;
	
	virtual void SetData(std::int32_t pixelWidth, std::int32_t pixelHeight,
		std::uint32_t levelCount, SurfaceFormat format, void const* pixelData) = 0;
	
	virtual void Apply(std::uint32_t index) = 0;
};

}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_RENDERSYSTEM_NATIVETEXTURE2D_309E0647_8B22_4554_A9E8_8269BC1035C5_HPP)
