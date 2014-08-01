//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_EFFECTREFLECTION_88543BAB_9612_40DD_9CD1_D6DB38D4D8DC_HPP
#define POMDOG_EFFECTREFLECTION_88543BAB_9612_40DD_9CD1_D6DB38D4D8DC_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "detail/ForwardDeclarations.hpp"
#include "EffectConstantDescription.hpp"
#include "Pomdog/Config/Export.hpp"
#include <memory>
#include <vector>

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeEffectReflection;

}// namespace RenderSystem
}// namespace Details

///@~Japanese
/// @brief エフェクトリフレクションです。
class POMDOG_EXPORT EffectReflection {
public:
	EffectReflection() = delete;
	EffectReflection(EffectReflection const&) = delete;
	EffectReflection(EffectReflection &&) = default;
	
	EffectReflection(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
		std::shared_ptr<EffectPass> const& effectPass);

	~EffectReflection();
	
	std::vector<EffectConstantDescription> GetConstantBuffers() const;
	
public:
	Details::RenderSystem::NativeEffectReflection* NativeEffectReflection();
	
private:
	std::unique_ptr<Details::RenderSystem::NativeEffectReflection> nativeEffectReflection;
};

}// namespace Pomdog

#endif // !defined(POMDOG_EFFECTREFLECTION_88543BAB_9612_40DD_9CD1_D6DB38D4D8DC_HPP)
