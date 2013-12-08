//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_NATIVESAMPLERSTATE_6CDE584F_6021_11E3_98C2_A8206655A22B_HPP
#define POMDOG_NATIVESAMPLERSTATE_6CDE584F_6021_11E3_98C2_A8206655A22B_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Config/Platform.hpp>

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeSamplerState
{
public:
	NativeSamplerState() = default;
	NativeSamplerState(NativeSamplerState const&) = delete;
	NativeSamplerState& operator=(NativeSamplerState const&) = delete;
	
	virtual ~NativeSamplerState() = default;
	
	///@~Japanese
	/// @brief レンダリングの直前にサンプラーステートを適用します。
	virtual void Apply(std::size_t index) = 0;
};

}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_NATIVESAMPLERSTATE_6CDE584F_6021_11E3_98C2_A8206655A22B_HPP)
