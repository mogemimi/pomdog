//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_RENDERSYSTEM_NATIVERENDERTARGET2D_9D701F83_BFE4_4D67_9305_A213A1DC7E77_HPP
#define POMDOG_SRC_RENDERSYSTEM_NATIVERENDERTARGET2D_9D701F83_BFE4_4D67_9305_A213A1DC7E77_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeRenderTarget2D
{
public:
	NativeRenderTarget2D() = default;
	NativeRenderTarget2D(NativeRenderTarget2D const&) = delete;
	NativeRenderTarget2D & operator=(NativeRenderTarget2D const&) = delete;

	virtual ~NativeRenderTarget2D() = default;
};

}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_RENDERSYSTEM_NATIVERENDERTARGET2D_9D701F83_BFE4_4D67_9305_A213A1DC7E77_HPP)
