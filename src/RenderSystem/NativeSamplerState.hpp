//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_NATIVESAMPLERSTATE_4A6E443C_3960_4E46_BEFA_2BE77ABC1AFD_HPP
#define POMDOG_NATIVESAMPLERSTATE_4A6E443C_3960_4E46_BEFA_2BE77ABC1AFD_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <cstdint>

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeSamplerState {
public:
	NativeSamplerState() = default;
	NativeSamplerState(NativeSamplerState const&) = delete;
	NativeSamplerState & operator=(NativeSamplerState const&) = delete;
	
	virtual ~NativeSamplerState() = default;
	
	///@~Japanese
	/// @brief レンダリングの直前にサンプラーステートを適用します。
	virtual void Apply(std::uint32_t index) = 0;
};

}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_NATIVESAMPLERSTATE_4A6E443C_3960_4E46_BEFA_2BE77ABC1AFD_HPP)
