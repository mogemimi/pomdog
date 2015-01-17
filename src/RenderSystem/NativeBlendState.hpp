//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_NATIVEBLENDSTATE_95C63681_C78E_45C7_AB01_365CD02D1E06_HPP
#define POMDOG_NATIVEBLENDSTATE_95C63681_C78E_45C7_AB01_365CD02D1E06_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeGraphicsContext;

class NativeBlendState {
public:
	NativeBlendState() = default;
	NativeBlendState(NativeBlendState const&) = delete;
	NativeBlendState & operator=(NativeBlendState const&) = delete;

	virtual ~NativeBlendState() = default;
	
	///@~Japanese
	/// @brief レンダリングの直前にブレンディングステートを適用します。
	virtual void Apply(NativeGraphicsContext & graphicsContext) = 0;
};

}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_NATIVEBLENDSTATE_95C63681_C78E_45C7_AB01_365CD02D1E06_HPP)
