//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_NATIVERENDERTARGET2D_9D701F83_BFE4_4D67_9305_A213A1DC7E77_HPP
#define POMDOG_NATIVERENDERTARGET2D_9D701F83_BFE4_4D67_9305_A213A1DC7E77_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include <cstdint>

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeRenderTarget2D {
public:
	NativeRenderTarget2D() = default;
	NativeRenderTarget2D(NativeRenderTarget2D const&) = delete;
	NativeRenderTarget2D & operator=(NativeRenderTarget2D const&) = delete;

	virtual ~NativeRenderTarget2D() = default;
	
	virtual void Apply(std::uint32_t index) = 0;
};

}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_NATIVERENDERTARGET2D_9D701F83_BFE4_4D67_9305_A213A1DC7E77_HPP)
