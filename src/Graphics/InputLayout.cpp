//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Graphics/InputLayout.hpp>
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "../RenderSystem/NativeInputLayout.hpp"
#include <Pomdog/Graphics/GraphicsDevice.hpp>
#include <Pomdog/Graphics/EffectPass.hpp>
#include <Pomdog/Utility/Assert.hpp>

namespace Pomdog {
//-----------------------------------------------------------------------
InputLayout::InputLayout(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
	std::shared_ptr<EffectPass> const& effectPass)
	//: nativeInputLayout(graphicsDevice->)
{
	auto nativeEffectPass = effectPass->NativeEffectPass();
	POMDOG_ASSERT(nativeEffectPass != nullptr);
	
	nativeInputLayout = graphicsDevice->NativeGraphicsDevice()->CreateInputLayout(
		*nativeEffectPass);
}
//-----------------------------------------------------------------------
InputLayout::InputLayout(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
	std::shared_ptr<EffectPass> const& effectPass,
	std::vector<VertexBufferBinding> const& vertexBindings)
{
	auto nativeEffectPass = effectPass->NativeEffectPass();
	POMDOG_ASSERT(nativeEffectPass != nullptr);

	nativeInputLayout = graphicsDevice->NativeGraphicsDevice()->CreateInputLayout(
		*nativeEffectPass, vertexBindings);
}
//-----------------------------------------------------------------------
InputLayout::~InputLayout() = default;
//-----------------------------------------------------------------------
Details::RenderSystem::NativeInputLayout* InputLayout::NativeInputLayout()
{
	return nativeInputLayout.get();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
