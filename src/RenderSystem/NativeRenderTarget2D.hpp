// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_NATIVERENDERTARGET2D_9D701F83_BFE4_4D67_9305_A213A1DC7E77_HPP
#define POMDOG_NATIVERENDERTARGET2D_9D701F83_BFE4_4D67_9305_A213A1DC7E77_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <cstdint>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {

class NativeRenderTarget2D {
public:
	NativeRenderTarget2D() = default;
	NativeRenderTarget2D(NativeRenderTarget2D const&) = delete;
	NativeRenderTarget2D & operator=(NativeRenderTarget2D const&) = delete;

	virtual ~NativeRenderTarget2D() = default;
};

}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_NATIVERENDERTARGET2D_9D701F83_BFE4_4D67_9305_A213A1DC7E77_HPP)
