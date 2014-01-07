//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_RENDERSYSTEM_NATIVEEFFECTREFLECTION_F923B192_157E_4D4E_BBBF_300ABEA0485A_HPP
#define POMDOG_SRC_RENDERSYSTEM_NATIVEEFFECTREFLECTION_F923B192_157E_4D4E_BBBF_300ABEA0485A_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <vector>

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class EffectBufferDescription;

class NativeEffectReflection
{
public:
	NativeEffectReflection() = default;
	NativeEffectReflection(NativeEffectReflection const&) = delete;
	NativeEffectReflection & operator=(NativeEffectReflection const&) = delete;
	
	virtual ~NativeEffectReflection() = default;

	virtual std::vector<EffectBufferDescription> GetConstantBuffers() const = 0;
};

}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_RENDERSYSTEM_NATIVEEFFECTREFLECTION_F923B192_157E_4D4E_BBBF_300ABEA0485A_HPP)
