//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_RENDERSYSTEM_NATIVEEFFECTPARAMETER_87DABE60_18BB_4C3B_84CB_7F3A9D85276C_HPP
#define POMDOG_SRC_RENDERSYSTEM_NATIVEEFFECTPARAMETER_87DABE60_18BB_4C3B_84CB_7F3A9D85276C_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Config/FundamentalTypes.hpp>

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeEffectParameter
{
public:
	NativeEffectParameter() = default;
	NativeEffectParameter(NativeEffectParameter const&) = delete;
	NativeEffectParameter& operator=(NativeEffectParameter const&) = delete;

	virtual ~NativeEffectParameter() = default;
};

}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_RENDERSYSTEM_NATIVEEFFECTPARAMETER_87DABE60_18BB_4C3B_84CB_7F3A9D85276C_HPP)
