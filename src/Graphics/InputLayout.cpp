//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Graphics/InputLayout.hpp>
#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Graphics/GraphicsDevice.hpp>
#include <Pomdog/Graphics/EffectPass.hpp>
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "../RenderSystem/NativeInputLayout.hpp"


namespace Pomdog {
//-----------------------------------------------------------------------
InputLayout::InputLayout(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
	std::shared_ptr<EffectPass> const& effectPass)
	//: nativeInputLayout(graphicsDevice->)
{
	auto nativeEffectPass = effectPass->GetNativeEffectPass();
	POMDOG_ASSERT(nativeEffectPass != nullptr);
	
	nativeInputLayout = graphicsDevice->GetNativeGraphicsDevice()->CreateInputLayout(
		nativeEffectPass
	);
}
//-----------------------------------------------------------------------
InputLayout::InputLayout(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
	std::shared_ptr<EffectPass> const& effectPass,
	std::vector<VertexBufferBinding> const& vertexBindings)
{
	auto nativeEffectPass = effectPass->GetNativeEffectPass();
	POMDOG_ASSERT(nativeEffectPass != nullptr);

	nativeInputLayout = graphicsDevice->GetNativeGraphicsDevice()->CreateInputLayout(
		nativeEffectPass,
		vertexBindings
	);
}
//-----------------------------------------------------------------------
InputLayout::~InputLayout()
{
}
//-----------------------------------------------------------------------
Details::RenderSystem::NativeInputLayout* InputLayout::GetNativeInputLayout()
{
	return nativeInputLayout.get();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
