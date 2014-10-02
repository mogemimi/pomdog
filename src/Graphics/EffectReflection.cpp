//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Graphics/EffectReflection.hpp>
#include "../RenderSystem/NativeEffectReflection.hpp"
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include <Pomdog/Graphics/GraphicsDevice.hpp>
#include <Pomdog/Graphics/EffectPass.hpp>
#include <Pomdog/Utility/Assert.hpp>

namespace Pomdog {
//-----------------------------------------------------------------------
EffectReflection::EffectReflection(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
	std::shared_ptr<EffectPass> const& effectPass)
{
	POMDOG_ASSERT(graphicsDevice && graphicsDevice->NativeGraphicsDevice());
	POMDOG_ASSERT(effectPass && effectPass->NativeEffectPass());
	nativeEffectReflection = graphicsDevice->NativeGraphicsDevice()->CreateEffectReflection(*effectPass->NativeEffectPass());
}
//-----------------------------------------------------------------------
EffectReflection::~EffectReflection() = default;
//-----------------------------------------------------------------------
std::vector<EffectConstantDescription> EffectReflection::GetConstantBuffers() const
{
	POMDOG_ASSERT(nativeEffectReflection);
	return nativeEffectReflection->GetConstantBuffers();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
