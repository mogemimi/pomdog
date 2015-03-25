// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_NATIVETEXTURE2D_309E0647_8B22_4554_A9E8_8269BC1035C5_HPP
#define POMDOG_NATIVETEXTURE2D_309E0647_8B22_4554_A9E8_8269BC1035C5_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <cstdint>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {

class NativeTexture2D {
public:
	NativeTexture2D() = default;
	NativeTexture2D(NativeTexture2D const&) = delete;
	NativeTexture2D & operator=(NativeTexture2D const&) = delete;

	virtual ~NativeTexture2D() = default;

	virtual void SetData(std::int32_t pixelWidth, std::int32_t pixelHeight,
		std::int32_t levelCount, SurfaceFormat format, void const* pixelData) = 0;
};

}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_NATIVETEXTURE2D_309E0647_8B22_4554_A9E8_8269BC1035C5_HPP)
